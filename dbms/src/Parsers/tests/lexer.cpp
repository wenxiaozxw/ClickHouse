#include <map>
#include <Parsers/Lexer.h>
#include <Core/Types.h>
#include <IO/ReadBufferFromFileDescriptor.h>
#include <IO/WriteBufferFromFileDescriptor.h>
#include <IO/ReadHelpers.h>
#include <IO/WriteHelpers.h>


/** How to test:
  * for i in ~/work/ClickHouse/dbms/tests/queries/0_stateless/*.sql; do echo $i; grep -q 'FORMAT' $i || ./lexer < $i || break; done
  */


using namespace DB;

std::map<TokenType, const char *> hilite =
{
    {TokenType::Whitespace, "\033[0;44m"},
    {TokenType::Comment, "\033[1;46m"},
    {TokenType::BareWord, "\033[1m"},
    {TokenType::Number, "\033[1;36m"},
    {TokenType::StringLiteral, "\033[1;32m"},
    {TokenType::QuotedIdentifier, "\033[1;35m"},

    {TokenType::OpeningRoundBracket, "\033[1;33m"},
    {TokenType::ClosingRoundBracket, "\033[1;33m"},
    {TokenType::OpeningSquareBracket, "\033[1;33m"},
    {TokenType::ClosingSquareBracket, "\033[1;33m"},

    {TokenType::Comma, "\033[1;33m"},
    {TokenType::Semicolon, "\033[1;33m"},
    {TokenType::Dot, "\033[1;33m"},
    {TokenType::Asterisk, "\033[1;33m"},
    {TokenType::Plus, "\033[1;33m"},
    {TokenType::Minus, "\033[1;33m"},
    {TokenType::Division, "\033[1;33m"},
    {TokenType::Modulo, "\033[1;33m"},
    {TokenType::Arrow, "\033[1;33m"},
    {TokenType::QuestionMark, "\033[1;33m"},
    {TokenType::Colon, "\033[1;33m"},
    {TokenType::Equals, "\033[1;33m"},
    {TokenType::NotEquals, "\033[1;33m"},
    {TokenType::Less, "\033[1;33m"},
    {TokenType::Greater, "\033[1;33m"},
    {TokenType::LessOrEquals, "\033[1;33m"},
    {TokenType::GreaterOrEquals, "\033[1;33m"},
    {TokenType::Concatenation, "\033[1;33m"},

    {TokenType::EndOfStream, ""},

    {TokenType::Error, "\033[0;41m"},
    {TokenType::ErrorMultilineCommentIsNotClosed, "\033[0;41m"},
    {TokenType::ErrorSingleQuoteIsNotClosed, "\033[0;41m"},
    {TokenType::ErrorDoubleQuoteIsNotClosed, "\033[0;41m"},
    {TokenType::ErrorBackQuoteIsNotClosed, "\033[0;41m"},
    {TokenType::ErrorSingleExclamationMark, "\033[0;41m"},
    {TokenType::ErrorWrongNumber, "\033[0;41m"},
};


int main(int argc, char ** argv)
{
    String query;
    ReadBufferFromFileDescriptor in(STDIN_FILENO);
    WriteBufferFromFileDescriptor out(STDOUT_FILENO);
    readStringUntilEOF(query, in);

    Lexer lexer(query.data(), query.data() + query.size());

    while (true)
    {
        Token token = lexer.nextToken();

        if (token.type == TokenType::EndOfStream)
            break;

        writeChar(' ', out);

        auto it = hilite.find(token.type);
        if (it != hilite.end())
            writeCString(it->second, out);

        writeString(token.begin, token.size(), out);

        if (it != hilite.end())
            writeCString("\033[0m", out);

        writeChar(' ', out);

        if (token.type >= TokenType::Error)
            return 1;
    }

    writeChar('\n', out);

    return 0;
}