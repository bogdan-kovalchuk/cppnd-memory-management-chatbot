#ifndef CHATBOT_TEST_HARNESS_H
#define CHATBOT_TEST_HARNESS_H

#include <string>
#include <vector>

#include "chatlogic.h"
#include "chatbot.h"
#include "graphnode.h"

inline std::vector<std::string> chatbotResponses;

ChatLogic::ChatLogic()
    : _currentNode(nullptr), _chatBot(nullptr), _panelDialog(nullptr)
{
}

ChatLogic::~ChatLogic() = default;

void ChatLogic::SetPanelDialogHandle(ChatBotPanelDialog *panelDialog)
{
    _panelDialog = panelDialog;
}

void ChatLogic::SetChatbotHandle(ChatBot *chatbot)
{
    _chatBot = chatbot;
}

void ChatLogic::SendMessageToChatbot(std::string message)
{
    if (_chatBot)
        _chatBot->ReceiveMessageFromUser(message);
}

void ChatLogic::SendMessageToUser(std::string message)
{
    chatbotResponses.push_back(std::move(message));
}

wxBitmap *ChatLogic::GetImageFromChatbot()
{
    return _chatBot ? _chatBot->GetImageHandle() : nullptr;
}

#endif
