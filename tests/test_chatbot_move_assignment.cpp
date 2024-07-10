#include <cassert>
#include <iostream>
#include <utility>

#include "chatbot_test_harness.h"

static void test_move_assignment_releases_destination_and_rebinds_chatlogic()
{
    ChatLogic sourceLogic;
    ChatBot source("source.png");
    source.SetChatLogicHandle(&sourceLogic);
    sourceLogic.SetChatbotHandle(&source);
    wxBitmap *sourceAvatar = source.GetImageHandle();

    ChatBot destination("destination.png");
    wxBitmap *destinationAvatar = destination.GetImageHandle();
    assert(wxBitmap::liveInstances == 2);

    destination = std::move(source);

    assert(destination.GetImageHandle() == sourceAvatar);
    assert(destination.GetImageHandle() != destinationAvatar);
    assert(source.GetImageHandle() == nullptr);
    assert(sourceLogic.GetImageFromChatbot() == sourceAvatar);
    assert(wxBitmap::liveInstances == 1);
}

static void test_self_move_assignment_preserves_avatar()
{
    ChatBot bot("avatar.png");
    wxBitmap *avatar = bot.GetImageHandle();
    ChatBot &sameBot = bot;

    bot = std::move(sameBot);

    assert(bot.GetImageHandle() == avatar);
}

int main()
{
    test_move_assignment_releases_destination_and_rebinds_chatlogic();
    test_self_move_assignment_preserves_avatar();
    assert(wxBitmap::liveInstances == 0);
    std::cout << "ChatBot move-assignment tests passed.\n";
    return 0;
}
