#include <cassert>
#include <iostream>
#include <new>

#include "chatbot_test_harness.h"

static void test_copy_constructs_a_distinct_avatar_and_rebinds_chatlogic()
{
    ChatLogic logic;
    ChatBot original("avatar.png");
    original.SetChatLogicHandle(&logic);
    logic.SetChatbotHandle(&original);

    ChatBot copied(original);

    assert(original.GetImageHandle() != nullptr);
    assert(copied.GetImageHandle() != nullptr);
    assert(copied.GetImageHandle() != original.GetImageHandle());
    assert(logic.GetImageFromChatbot() == copied.GetImageHandle());
}

static void test_failed_copy_construction_keeps_existing_registration()
{
    ChatLogic logic;
    ChatBot original("avatar.png");
    original.SetChatLogicHandle(&logic);
    logic.SetChatbotHandle(&original);

    wxBitmap::throwOnCopy = true;
    bool threw = false;
    try
    {
        ChatBot copy(original);
    }
    catch (const std::bad_alloc &)
    {
        threw = true;
    }
    wxBitmap::throwOnCopy = false;

    assert(threw);
    assert(logic.GetImageFromChatbot() == original.GetImageHandle());
}

int main()
{
    test_copy_constructs_a_distinct_avatar_and_rebinds_chatlogic();
    test_failed_copy_construction_keeps_existing_registration();
    assert(wxBitmap::liveInstances == 0);
    std::cout << "ChatBot copy-constructor tests passed.\n";
    return 0;
}
