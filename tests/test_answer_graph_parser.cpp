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

int main()
{
    std::cout << "Answer graph parser tests:\n";
    test_parses_node_answers();
    std::cout << "All answer graph parser tests passed.\n";
    return 0;
}
