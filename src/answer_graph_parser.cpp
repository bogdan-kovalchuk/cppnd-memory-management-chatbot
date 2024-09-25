#include "answer_graph_parser.h"

#include <charconv>
#include <cctype>
#include <optional>
#include <unordered_set>
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

bool ContainsNonWhitespace(const std::string &text)
{
    for (const unsigned char character : text)
    {
        if (!std::isspace(character))
            return true;
    }
    return false;
}

bool ContainsUnexpectedText(const std::string &text)
{
    const std::size_t comment = text.find("//");
    return ContainsNonWhitespace(text.substr(0, comment));
}

std::vector<Token> ParseTokens(
    const std::string &line,
    std::size_t lineNumber,
    AnswerGraphParseResult &result,
    bool &wellFormed
)
{
    std::vector<Token> tokens;
    wellFormed = true;
    std::size_t cursor = 0;
    while (true)
    {
        const std::size_t front = line.find('<', cursor);
        if (front == std::string::npos)
        {
            const std::string remainder = line.substr(cursor);
            if (tokens.empty() && ContainsNonWhitespace(remainder)
                && remainder.find("//") != 0)
            {
                result.issues.push_back(
                    {lineNumber, "line contains text but no answer-graph tokens"}
                );
            }
            else if (!tokens.empty() && ContainsUnexpectedText(remainder))
            {
                result.issues.push_back(
                    {lineNumber, "line contains text outside answer-graph tokens"}
                );
                wellFormed = false;
            }
            break;
        }
        if (ContainsUnexpectedText(line.substr(cursor, front - cursor)))
        {
            result.issues.push_back(
                {lineNumber, "line contains text outside answer-graph tokens"}
            );
            wellFormed = false;
            break;
        }
        const std::size_t back = line.find('>', front + 1);
        if (back == std::string::npos)
        {
            result.issues.push_back({lineNumber, "token is missing closing '>'"});
            wellFormed = false;
            break;
        }
        const std::string token = line.substr(front + 1, back - front - 1);
        const std::size_t separator = token.find(':');
        if (separator != std::string::npos)
        {
            tokens.emplace_back(
                token.substr(0, separator), token.substr(separator + 1)
            );
        }
        else
        {
            result.issues.push_back({lineNumber, "token is missing ':'"});
            wellFormed = false;
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
    std::unordered_set<int> nodeIds;
    std::unordered_set<int> edgeIds;
    while (std::getline(input, line))
    {
        ++lineNumber;
        bool tokensWellFormed = false;
        const std::vector<Token> tokens = ParseTokens(
            line, lineNumber, result, tokensWellFormed
        );
        if (tokens.empty() || !tokensWellFormed)
            continue;
        const std::string *type = FindToken(tokens, "TYPE");
        if (!type)
        {
            result.issues.push_back({lineNumber, "record is missing TYPE"});
            continue;
        }
        if (*type == "NODE")
        {
            const std::string *idText = FindToken(tokens, "ID");
            const std::optional<int> id = idText ? ParseInteger(*idText) : std::nullopt;
            if (!id)
            {
                result.issues.push_back({lineNumber, "NODE requires an integer ID"});
                continue;
            }
            if (!nodeIds.insert(*id).second)
            {
                result.issues.push_back({lineNumber, "duplicate NODE ID"});
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
        else if (*type == "EDGE")
        {
            const std::string *idText = FindToken(tokens, "ID");
            const std::string *parentText = FindToken(tokens, "PARENT");
            const std::string *childText = FindToken(tokens, "CHILD");
            const std::optional<int> id = idText ? ParseInteger(*idText) : std::nullopt;
            const std::optional<int> parent = parentText ? ParseInteger(*parentText) : std::nullopt;
            const std::optional<int> child = childText ? ParseInteger(*childText) : std::nullopt;
            if (!id || !parent || !child)
            {
                result.issues.push_back(
                    {lineNumber, "EDGE requires integer ID, PARENT, and CHILD values"}
                );
                continue;
            }
            if (!edgeIds.insert(*id).second)
            {
                result.issues.push_back({lineNumber, "duplicate EDGE ID"});
                continue;
            }
            ParsedAnswerEdge edge{*id, *parent, *child, {}};
            for (const auto &token : tokens)
            {
                if (token.first == "KEYWORD")
                    edge.keywords.push_back(token.second);
            }
            result.edges.push_back(std::move(edge));
        }
        else
        {
            result.issues.push_back({lineNumber, "unsupported TYPE: " + *type});
        }
    }
    return result;
}
