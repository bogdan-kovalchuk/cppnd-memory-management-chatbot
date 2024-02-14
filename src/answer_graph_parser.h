#ifndef ANSWER_GRAPH_PARSER_H
#define ANSWER_GRAPH_PARSER_H

#include <istream>
#include <string>
#include <vector>

struct ParsedAnswerNode
{
    int id;
    std::vector<std::string> answers;
};

struct AnswerGraphParseIssue
{
    std::size_t line;
    std::string message;
};

struct AnswerGraphParseResult
{
    std::vector<ParsedAnswerNode> nodes;
    std::vector<AnswerGraphParseIssue> issues;
};

AnswerGraphParseResult ParseAnswerGraph(std::istream &input);

#endif
