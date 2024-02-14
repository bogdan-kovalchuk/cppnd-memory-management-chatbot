#include "answer_graph_parser.h"

#include <charconv>
#include <optional>
#include <utility>

namespace
{
using Token = std::pair<std::string, std::string>;

std::optional<int> ParseInteger(const std::string &text)
{
    int value = 0;
    const char *begin = text.data();
    const char *end = begin + text.size();
    const auto result = std::from_chars(begin, end, value);
    if (result.ec != std::errc{} || result.ptr != end)
        return std::nullopt;
    return value;
}

std::vector<Token> ParseTokens(const std::string &line)
{
    std::vector<Token> tokens;
    std::size_t cursor = 0;
    while (true)
    {
        const std::size_t front = line.find('<', cursor);
        if (front == std::string::npos)
            break;
        const std::size_t back = line.find('>', front + 1);
        if (back == std::string::npos)
            break;
        const std::string token = line.substr(front + 1, back - front - 1);
        const std::size_t separator = token.find(':');
        if (separator != std::string::npos)
        {
            tokens.emplace_back(
                token.substr(0, separator), token.substr(separator + 1)
            );
        }
        cursor = back + 1;
    }
    return tokens;
}

const std::string *FindToken(const std::vector<Token> &tokens, const std::string &name)
{
    for (const auto &token : tokens)
    {
        if (token.first == name)
            return &token.second;
    }
    return nullptr;
}
}

AnswerGraphParseResult ParseAnswerGraph(std::istream &input)
{
    AnswerGraphParseResult result;
    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(input, line))
    {
        ++lineNumber;
        const std::vector<Token> tokens = ParseTokens(line);
        const std::string *type = FindToken(tokens, "TYPE");
        if (!type || *type != "NODE")
            continue;
        const std::string *idText = FindToken(tokens, "ID");
        const std::optional<int> id = idText ? ParseInteger(*idText) : std::nullopt;
        if (!id)
        {
            result.issues.push_back({lineNumber, "NODE requires an integer ID"});
            continue;
        }

        ParsedAnswerNode node{*id, {}};
        for (const auto &token : tokens)
        {
            if (token.first == "ANSWER")
                node.answers.push_back(token.second);
        }
        result.nodes.push_back(std::move(node));
    }
    return result;
}
