#include <cassert>
#include <iostream>
#include <utility>

#include "chatbot_test_harness.h"

static void test_move_construction_transfers_avatar_and_registration()
{
    ChatLogic logic;
    ChatBot original("avatar.png");
    original.SetChatLogicHandle(&logic);
    logic.SetChatbotHandle(&original);
    wxBitmap *avatar = original.GetImageHandle();

    ChatBot moved(std::move(original));

    assert(moved.GetImageHandle() == avatar);
    assert(original.GetImageHandle() == nullptr);
    assert(logic.GetImageFromChatbot() == avatar);
}

static void test_moved_from_bot_can_be_moved_again()
{
    ChatBot original("avatar.png");
    ChatBot first(std::move(original));
    ChatBot second(std::move(original));

    assert(first.GetImageHandle() != nullptr);
    assert(second.GetImageHandle() == nullptr);
    assert(original.GetImageHandle() == nullptr);
}

int main()
{
    test_move_construction_transfers_avatar_and_registration();
    test_moved_from_bot_can_be_moved_again();
    assert(wxBitmap::liveInstances == 0);
    std::cout << "ChatBot move-constructor tests passed.\n";
    return 0;
}
