#include <cassert>
#include <iostream>
#include <new>

#include "chatbot_test_harness.h"

static void test_copy_assignment_replaces_avatar_and_rebinds_chatlogic()
{
    ChatLogic sourceLogic;
    ChatLogic destinationLogic;
    ChatBot source("source.png");
    ChatBot destination("destination.png");
    source.SetChatLogicHandle(&sourceLogic);
    destination.SetChatLogicHandle(&destinationLogic);
    sourceLogic.SetChatbotHandle(&source);
    destinationLogic.SetChatbotHandle(&destination);
    wxBitmap *sourceAvatar = source.GetImageHandle();
    wxBitmap *destinationAvatar = destination.GetImageHandle();

    destination = source;

    assert(destination.GetImageHandle() != sourceAvatar);
    assert(destination.GetImageHandle() != destinationAvatar);
    assert(source.GetImageHandle() == sourceAvatar);
    assert(sourceLogic.GetImageFromChatbot() == destination.GetImageHandle());
}

static void test_failed_copy_assignment_preserves_destination_and_registration()
{
    ChatLogic sourceLogic;
    ChatLogic destinationLogic;
    ChatBot source("source.png");
    ChatBot destination("destination.png");
    source.SetChatLogicHandle(&sourceLogic);
    destination.SetChatLogicHandle(&destinationLogic);
    sourceLogic.SetChatbotHandle(&source);
    destinationLogic.SetChatbotHandle(&destination);
    wxBitmap *destinationAvatar = destination.GetImageHandle();

    wxBitmap::throwOnCopy = true;
    bool threw = false;
    try
    {
        destination = source;
    }
    catch (const std::bad_alloc &)
    {
        threw = true;
    }
    wxBitmap::throwOnCopy = false;

    assert(threw);
    assert(destination.GetImageHandle() == destinationAvatar);
    assert(destinationLogic.GetImageFromChatbot() == destinationAvatar);
    assert(sourceLogic.GetImageFromChatbot() == source.GetImageHandle());
}

static void test_self_copy_assignment_preserves_avatar()
{
    ChatBot bot("avatar.png");
    wxBitmap *avatar = bot.GetImageHandle();

    bot = bot;

    assert(bot.GetImageHandle() == avatar);
}

int main()
{
    test_copy_assignment_replaces_avatar_and_rebinds_chatlogic();
    test_failed_copy_assignment_preserves_destination_and_registration();
    test_self_copy_assignment_preserves_avatar();
    assert(wxBitmap::liveInstances == 0);
    std::cout << "ChatBot copy-assignment tests passed.\n";
    return 0;
}
