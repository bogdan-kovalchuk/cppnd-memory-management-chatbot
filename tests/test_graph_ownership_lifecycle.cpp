#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "chatlogic.h"

void ChatBotPanelDialog::PrintChatbotResponse(std::string)
{
}

static std::filesystem::path writeAnswerGraph()
{
    const std::filesystem::path path =
        std::filesystem::temp_directory_path() / "chatbot-ownership-lifecycle.txt";
    std::ofstream graph(path);
    graph << "<TYPE:NODE><ID:1><ANSWER:root>\n";
    graph << "<TYPE:NODE><ID:2><ANSWER:child>\n";
    graph << "<TYPE:EDGE><ID:3><PARENT:1><CHILD:2><KEYWORD:go>\n";
    graph.close();
    return path;
}

static void test_chatlogic_owns_graph_and_follows_chatbot_transfer()
{
    const std::filesystem::path graphPath = writeAnswerGraph();
    {
        ChatLogic logic;
        logic.LoadAnswerGraphFromFile(graphPath.string());

        assert(logic.GetImageFromChatbot() != nullptr);
        assert(wxBitmap::liveInstances == 1);

        logic.SendMessageToChatbot("go");

        assert(logic.GetImageFromChatbot() != nullptr);
        assert(wxBitmap::liveInstances == 1);
    }
    std::filesystem::remove(graphPath);

    assert(wxBitmap::liveInstances == 0);
}

int main()
{
    test_chatlogic_owns_graph_and_follows_chatbot_transfer();
    std::cout << "Graph ownership lifecycle tests passed.\n";
    return 0;
}
