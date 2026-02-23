#include <gtest/gtest.h>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include "builtins.hpp"
#include "command_parser.hpp"
#include "execution_context.hpp"
#include "lexer.hpp"
#include "pipe_executor.hpp"

namespace fluffy_tribble {
namespace {

class TempFile {
public:
    TempFile(const std::string &content) {
        path = testing::TempDir() + "grep_test_XXXXXX";
        std::vector<char> buf(path.begin(), path.end());
        buf.push_back('\0');
        int fd = mkstemp(buf.data());
        if (fd == -1) {
            throw std::runtime_error("Cannot create temporary file");
        }
        close(fd);
        path = buf.data();
        std::ofstream out(path);
        out << content;
    }

    ~TempFile() {
        std::remove(path.c_str());
    }

    [[nodiscard]] const std::string &name() const {
        return path;
    }

private:
    std::string path;
};

static std::string run_pipe(const std::string &line) {
    ExecutionContext ctx;
    Lexer lexer;
    CommandParser parser;
    std::istringstream in;
    std::ostringstream out, err;

    auto tokens = lexer.tokenize(line, ctx);
    auto pipe = parser.parse(tokens);
    PipeExecutor::execute(pipe, in, out, err, ctx);
    return out.str();
}

TEST(GrepTest, EmptyPattern) {
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    std::vector<std::string> args = {"grep"};

    run<CommandID::GREP>(args, in, out, err, ctx);
    EXPECT_EQ(ctx.last_status(), 2);
    EXPECT_EQ(
        err.str(), "grep: pattern: 1 argument(s) expected. 0 provided.\n"
    );
}

TEST(GrepTest, StdinBasic) {
    ExecutionContext ctx;
    std::istringstream in("hello world\nthis is a test\nhello again\n");
    std::ostringstream out, err;
    std::vector<std::string> args = {"grep", "hello"};

    run<CommandID::GREP>(args, in, out, err, ctx);
    EXPECT_EQ(ctx.last_status(), 0);
    EXPECT_TRUE(err.str().empty());
    EXPECT_EQ(out.str(), "hello world\nhello again\n");
}

TEST(GrepTest, SingleFile) {
    TempFile file("apple\nbanana\napple pie\ncherry\n");
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    std::vector<std::string> args = {"grep", "apple", file.name()};

    run<CommandID::GREP>(args, in, out, err, ctx);
    EXPECT_EQ(ctx.last_status(), 0);
    EXPECT_TRUE(err.str().empty());
    EXPECT_EQ(out.str(), "apple\napple pie\n");
}

TEST(GrepTest, MultipleFiles) {
    TempFile file1("alpha\nbeta\ngamma\n");
    TempFile file2("delta\nbeta\nomega\n");
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    std::vector<std::string> args = {
        "grep", "beta", file1.name(), file2.name()
    };

    run<CommandID::GREP>(args, in, out, err, ctx);
    EXPECT_EQ(ctx.last_status(), 0);
    EXPECT_TRUE(err.str().empty());
    EXPECT_EQ(out.str(), "beta\nbeta\n");
}

TEST(GrepTest, WordRegexpOption) {
    TempFile file("test\ntesting\natest\n test \n");
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    std::vector<std::string> args = {"grep", "-w", "test", file.name()};

    run<CommandID::GREP>(args, in, out, err, ctx);
    EXPECT_EQ(ctx.last_status(), 0);
    EXPECT_TRUE(err.str().empty());
    EXPECT_EQ(out.str(), "test\n test \n");
}

TEST(GrepTest, IgnoreCaseOption) {
    TempFile file("Hello\nHELLO\nhello\nhElLo\nworld\n");
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    std::vector<std::string> args = {"grep", "-i", "hello", file.name()};

    run<CommandID::GREP>(args, in, out, err, ctx);
    EXPECT_EQ(ctx.last_status(), 0);
    EXPECT_TRUE(err.str().empty());
    EXPECT_EQ(out.str(), "Hello\nHELLO\nhello\nhElLo\n");
}

TEST(GrepTest, AfterContextOption) {
    TempFile file("line1\nmatch1\nline2\nline3\nmatch2\nline4\n");
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    std::vector<std::string> args = {"grep", "-A", "2", "match", file.name()};

    run<CommandID::GREP>(args, in, out, err, ctx);
    EXPECT_EQ(ctx.last_status(), 0);
    EXPECT_TRUE(err.str().empty());
    std::string expected =
        "match1\n"
        "line2\n"
        "line3\n"
        "match2\n"
        "line4\n";
    EXPECT_EQ(out.str(), expected);
}

TEST(GrepTest, CombinedOptions) {
    TempFile file("Test line\ntest\nTESTING\nanother test\n");
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    std::vector<std::string> args = {"grep", "-i", "-w", "test", file.name()};

    run<CommandID::GREP>(args, in, out, err, ctx);
    EXPECT_EQ(ctx.last_status(), 0);
    EXPECT_TRUE(err.str().empty());
    EXPECT_EQ(out.str(), "Test line\ntest\nanother test\n");
}

TEST(GrepTest, NonexistentFile) {
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    std::vector<std::string> args = {
        "grep", "pattern", "nonexistent_file_xyz.txt"
    };

    run<CommandID::GREP>(args, in, out, err, ctx);
    EXPECT_EQ(ctx.last_status(), 1);
    EXPECT_NE(err.str().find("No such file or directory"), std::string::npos);
}

TEST(GrepTest, UnknownOption) {
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    std::vector<std::string> args = {"grep", "-z", "pattern"};

    run<CommandID::GREP>(args, in, out, err, ctx);
    EXPECT_EQ(ctx.last_status(), 2);
    EXPECT_EQ(err.str(), "grep: Unknown argument: -z\n");
}

TEST(GrepTest, MissingArgumentForA) {
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    std::vector<std::string> args = {"grep", "-A", "pattern"};

    run<CommandID::GREP>(args, in, out, err, ctx);
    EXPECT_EQ(ctx.last_status(), 2);
    EXPECT_EQ(
        err.str(),
        "grep: Failed to parse 'pattern' as decimal integer: pattern 'pattern' "
        "not found\n"
    );
}

TEST(GrepTest, MissingArgumentForA1) {
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    std::vector<std::string> args = {"grep", "-A1", "pattern"};

    run<CommandID::GREP>(args, in, out, err, ctx);
    EXPECT_EQ(ctx.last_status(), 2);
    EXPECT_EQ(
        err.str(),
        "grep: Failed to parse 'pattern' as decimal integer: pattern 'pattern' "
        "not found\n"
    );
}

TEST(GrepTest, CombinedOptionsWi) {
    TempFile file("Test line\ntest\nTESTING\nanother test\n");
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    std::vector<std::string> args = {"grep", "-wi", "test", file.name()};

    run<CommandID::GREP>(args, in, out, err, ctx);
    EXPECT_EQ(ctx.last_status(), 0);
    EXPECT_TRUE(err.str().empty());
    EXPECT_EQ(out.str(), "Test line\ntest\nanother test\n");
}

TEST(GrepTest, CombinedOptionsIw) {
    TempFile file("Test line\ntest\nTESTING\nanother test\n");
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    std::vector<std::string> args = {"grep", "-iw", "test", file.name()};

    run<CommandID::GREP>(args, in, out, err, ctx);
    EXPECT_EQ(ctx.last_status(), 0);
    EXPECT_TRUE(err.str().empty());
    EXPECT_EQ(out.str(), "Test line\ntest\nanother test\n");
}

TEST(GrepTest, CombinedOptionsWai) {
    TempFile file("testing\nTest line\nTEST\nmy test\n");
    ExecutionContext ctx;
    std::istringstream in;
    std::ostringstream out, err;
    std::vector<std::string> args = {"grep", "-wAi", "1", "test", file.name()};

    run<CommandID::GREP>(args, in, out, err, ctx);
    EXPECT_EQ(ctx.last_status(), 0);
    EXPECT_TRUE(err.str().empty());
    std::string expected =
        "Test line\n"
        "TEST\n"
        "my test\n";
    EXPECT_EQ(out.str(), expected);
}

TEST(GrepPipeTest, CatPipeGrepBasic) {
    TempFile f("aba\ncaba\nfoo\n");
    EXPECT_EQ(run_pipe("cat " + f.name() + " | grep ca"), "caba\n");
}

TEST(GrepPipeTest, CatPipeGrepNoMatch) {
    TempFile f("aba\ncaba\nfoo\n");
    EXPECT_EQ(run_pipe("cat " + f.name() + " | grep xyz"), "");
}

TEST(GrepPipeTest, CatPipeGrepMultipleMatches) {
    TempFile f("apple\napricot\nbanana\navocado\n");
    EXPECT_EQ(
        run_pipe("cat " + f.name() + " | grep a"),
        "apple\napricot\nbanana\navocado\n"
    );
}

TEST(GrepPipeTest, CatPipeGrepIgnoreCase) {
    TempFile f("Hello\nWORLD\nhello world\nbye\n");
    EXPECT_EQ(
        run_pipe("cat " + f.name() + " | grep -i HELLO"), "Hello\nhello world\n"
    );
}

TEST(GrepPipeTest, CatPipeGrepWordRegexp) {
    TempFile f("test\ntesting\na test b\ncontest\n");
    EXPECT_EQ(
        run_pipe("cat " + f.name() + " | grep -w test"), "test\na test b\n"
    );
}

TEST(GrepPipeTest, CatPipeGrepAfterContext) {
    TempFile f("aaa\nmatch\nbbb\nccc\nmatch\nddd\n");
    EXPECT_EQ(
        run_pipe("cat " + f.name() + " | grep -A 1 match"),
        "match\nbbb\nmatch\nddd\n"
    );
}

TEST(GrepPipeTest, CatPipeGrepWordAndIgnoreCase) {
    TempFile f("Go\nGolang\ngo lang\ngo\n");
    EXPECT_EQ(
        run_pipe("cat " + f.name() + " | grep -i -w go"), "Go\ngo lang\ngo\n"
    );
}

TEST(GrepPipeTest, GrepInMiddleOfPipeThenWc) {
    TempFile f("alpha\nbeta\ngamma\nalpha beta\n");
    EXPECT_EQ(run_pipe("cat " + f.name() + " | grep alpha | wc"), "2 3 17\n");
}

TEST(GrepPipeTest, GrepInMiddleThenCat) {
    TempFile f("foo\nbar\nbaz\n");
    EXPECT_EQ(run_pipe("cat " + f.name() + " | grep ba | cat"), "bar\nbaz\n");
}

TEST(GrepPipeTest, DoubleGrep) {
    TempFile f("ab\nac\nbc\nabc\nxyz\n");
    EXPECT_EQ(run_pipe("cat " + f.name() + " | grep a | grep b"), "ab\nabc\n");
}

TEST(GrepPipeTest, DoubleGrepNoMatch) {
    TempFile f("hello\nworld\ntest\n");
    EXPECT_EQ(run_pipe("cat " + f.name() + " | grep hello | grep xyz"), "");
}

TEST(GrepPipeTest, TripleGrep) {
    TempFile f("abc\nabc def\nabc def ghi\ndef ghi\nghi\n");
    EXPECT_EQ(
        run_pipe("cat " + f.name() + " | grep abc | grep def | grep ghi"),
        "abc def ghi\n"
    );
}

TEST(GrepPipeTest, DoubleGrepIgnoreCaseAndWord) {
    TempFile f("Hello World\nhello\nworld Hello\nfoo\n");
    EXPECT_EQ(
        run_pipe("cat " + f.name() + " | grep -i hello | grep -i -w world"),
        "Hello World\nworld Hello\n"
    );
}

TEST(GrepPipeTest, GrepRegexDigits) {
    TempFile f("foo123\nbar\nbaz456\nqux\n");
    EXPECT_EQ(
        run_pipe("cat " + f.name() + " | grep '[0-9]'"), "foo123\nbaz456\n"
    );
}

TEST(GrepPipeTest, GrepRegexStartAnchor) {
    TempFile f("start here\nend start\njust start\n");
    EXPECT_EQ(run_pipe("cat " + f.name() + " | grep '^start'"), "start here\n");
}

TEST(GrepPipeTest, GrepRegexEndAnchor) {
    TempFile f("line end\nend\nmiddle end more\n");
    EXPECT_EQ(
        run_pipe("cat " + f.name() + " | grep 'end$'"), "line end\nend\n"
    );
}

TEST(GrepPipeTest, GrepStarQuantifier) {
    TempFile f("кавычки\nкавыч\nкавычч\nпросто текст\n");
    EXPECT_EQ(
        run_pipe("cat " + f.name() + " | grep 'кавыч*'"),
        "кавычки\nкавыч\nкавычч\n"
    );
}

TEST(GrepPipeTest, GrepAlternation) {
    TempFile f("cat\ndog\nbird\ncod\n");
    EXPECT_EQ(run_pipe("cat " + f.name() + " | grep 'cat|dog'"), "cat\ndog\n");
}

TEST(GrepPipeTest, LongPipelineWithGrep) {
    TempFile f("aabb\naa\nbb\naabb cc\ncc\n");
    EXPECT_EQ(
        run_pipe("cat " + f.name() + " | grep aa | cat | grep bb | wc"),
        "2 3 13\n"
    );
}

TEST(GrepPipeTest, GrepAfterContextThenWc) {
    TempFile f("x\nmatch\ny\nz\n");
    EXPECT_EQ(
        run_pipe("cat " + f.name() + " | grep -A 1 match | wc"), "2 2 8\n"
    );
}

TEST(GrepPipeTest, GrepEmptyResultPipedToWc) {
    TempFile f("foo\nbar\n");
    EXPECT_EQ(run_pipe("cat " + f.name() + " | grep xyz | wc"), "0 0 0\n");
}

}  // namespace
}  // namespace fluffy_tribble