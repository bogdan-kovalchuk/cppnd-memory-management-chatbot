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

int main()
{
    std::cout << "Answer graph parser tests:\n";
    test_parses_node_answers();
    test_parses_edge_relationships_and_keywords();
    std::cout << "All answer graph parser tests passed.\n";
    return 0;
}
