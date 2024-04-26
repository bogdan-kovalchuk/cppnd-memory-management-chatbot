#include <cassert>
#include <iostream>
#include <sstream>

#include "answer_graph_parser.h"

static void test_parses_node_answers()
{
    std::istringstream input(
        "<TYPE:NODE><ID:7><ANSWER:first><ANSWER:second>\n"
        "<TYPE:NODE><ID:9><ANSWER:last>\n"
    );

    const AnswerGraphParseResult result = ParseAnswerGraph(input);

    assert(result.issues.empty());
    assert(result.nodes.size() == 2);
    assert(result.nodes[0].id == 7);
    assert(result.nodes[0].answers.size() == 2);
    assert(result.nodes[0].answers[0] == "first");
    assert(result.nodes[0].answers[1] == "second");
    assert(result.nodes[1].id == 9);
    assert(result.nodes[1].answers[0] == "last");
    std::cout << "  PASS: node records preserve IDs and answers\n";
}

static void test_parses_edge_relationships_and_keywords()
{
    std::istringstream input(
        "<TYPE:EDGE><ID:12><PARENT:7><CHILD:9>"
        "<KEYWORD:memory><KEYWORD:ownership>\n"
    );

    const AnswerGraphParseResult result = ParseAnswerGraph(input);

    assert(result.issues.empty());
    assert(result.edges.size() == 1);
    assert(result.edges[0].id == 12);
    assert(result.edges[0].parentId == 7);
    assert(result.edges[0].childId == 9);
    assert(result.edges[0].keywords.size() == 2);
    assert(result.edges[0].keywords[0] == "memory");
    assert(result.edges[0].keywords[1] == "ownership");
    std::cout << "  PASS: edge records preserve endpoints and keywords\n";
}

static void test_reports_malformed_records_with_line_numbers()
{
    std::istringstream input(
        "plain text\n"
        "<TYPE:NODE><ID:not-a-number>\n"
        "<TYPE:EDGE><ID:1><PARENT:x><CHILD:2>\n"
        "<TYPE:NODE><ID:4\n"
        "<ID:5>\n"
        "<TYPE:OTHER><ID:6>\n"
    );

    const AnswerGraphParseResult result = ParseAnswerGraph(input);

    assert(result.nodes.empty());
    assert(result.edges.empty());
    assert(result.issues.size() == 6);
    for (std::size_t index = 0; index < result.issues.size(); ++index)
        assert(result.issues[index].line == index + 1);
    assert(result.issues[0].message.find("no answer-graph tokens") != std::string::npos);
    assert(result.issues[1].message.find("integer ID") != std::string::npos);
    assert(result.issues[2].message.find("integer ID, PARENT, and CHILD") != std::string::npos);
    assert(result.issues[3].message.find("closing") != std::string::npos);
    assert(result.issues[4].message.find("missing TYPE") != std::string::npos);
    assert(result.issues[5].message.find("unsupported TYPE") != std::string::npos);
    std::cout << "  PASS: malformed records include actionable line diagnostics\n";
}

static void test_rejects_duplicate_node_and_edge_ids()
{
    std::istringstream input(
        "<TYPE:NODE><ID:1><ANSWER:first>\n"
        "<TYPE:NODE><ID:1><ANSWER:duplicate>\n"
        "<TYPE:EDGE><ID:10><PARENT:1><CHILD:1><KEYWORD:first>\n"
        "<TYPE:EDGE><ID:10><PARENT:1><CHILD:1><KEYWORD:duplicate>\n"
    );

    const AnswerGraphParseResult result = ParseAnswerGraph(input);

    assert(result.nodes.size() == 1);
    assert(result.nodes[0].answers[0] == "first");
    assert(result.edges.size() == 1);
    assert(result.edges[0].keywords[0] == "first");
    assert(result.issues.size() == 2);
    assert(result.issues[0].line == 2);
    assert(result.issues[0].message == "duplicate NODE ID");
    assert(result.issues[1].line == 4);
    assert(result.issues[1].message == "duplicate EDGE ID");
    std::cout << "  PASS: duplicate IDs are diagnosed and skipped\n";
}

int main()
{
    std::cout << "Answer graph parser tests:\n";
    test_parses_node_answers();
    test_parses_edge_relationships_and_keywords();
    test_reports_malformed_records_with_line_numbers();
    test_rejects_duplicate_node_and_edge_ids();
    std::cout << "All answer graph parser tests passed.\n";
    return 0;
}
