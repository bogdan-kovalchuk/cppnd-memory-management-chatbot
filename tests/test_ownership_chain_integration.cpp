#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

// Model of the real ChatLogic -> GraphNode -> ChatBot ownership chain:
//   - ChatLogicModel owns its NodeModels via unique_ptr (src/chatlogic.h _nodes).
//   - Each NodeModel owns one ChatBotModel by value (src/graphnode.h _chatBot);
//     the bot is relocated between nodes via swap-based MoveChatbotHere, never
//     duplicated (src/graphnode.cpp).
//   - ChatBotModel holds a non-owning back-pointer to its ChatLogicModel
//     (src/chatbot.h _chatLogic) and re-registers itself with
//     ChatLogicModel::setChatbotHandle on every copy/move (src/chatbot.cpp),
//     which is how ChatLogic's non-owning "current bot" pointer stays valid
//     even though the bot's address changes every time it moves to a new node.

struct MockImage {
    char *data;
    static int alive;

    MockImage() : data(nullptr) {}
    explicit MockImage(const char *s) {
        size_t n = std::strlen(s) + 1;
        data = new char[n];
        std::memcpy(data, s, n);
        ++alive;
    }
    ~MockImage() {
        if (data) --alive;
        delete[] data;
    }
    MockImage(const MockImage &o) {
        if (o.data) {
            size_t n = std::strlen(o.data) + 1;
            data = new char[n];
            std::memcpy(data, o.data, n);
            ++alive;
        } else {
            data = nullptr;
        }
    }
    MockImage &operator=(const MockImage &o) {
        if (this != &o) {
            if (data) --alive;
            delete[] data;
            if (o.data) {
                size_t n = std::strlen(o.data) + 1;
                data = new char[n];
                std::memcpy(data, o.data, n);
                ++alive;
            } else {
                data = nullptr;
            }
        }
        return *this;
    }
    MockImage(MockImage &&o) noexcept : data(o.data) { o.data = nullptr; }
    MockImage &operator=(MockImage &&o) noexcept {
        if (this != &o) {
            if (data) --alive;
            delete[] data;
            data = o.data;
            o.data = nullptr;
        }
        return *this;
    }
};
int MockImage::alive = 0;

struct ChatLogicModel;
struct NodeModel;

struct ChatBotModel {
    MockImage image;
    NodeModel *currentNode;
    ChatLogicModel *chatLogic;

    ChatBotModel() : currentNode(nullptr), chatLogic(nullptr) {}
    explicit ChatBotModel(const char *filename)
        : image(filename), currentNode(nullptr), chatLogic(nullptr) {}

    ChatBotModel(const ChatBotModel &o)
        : image(o.image), currentNode(o.currentNode), chatLogic(o.chatLogic) {
        registerWithChatLogic();
    }
    ChatBotModel &operator=(const ChatBotModel &o) {
        if (this == &o) return *this;
        image = o.image;
        currentNode = o.currentNode;
        chatLogic = o.chatLogic;
        registerWithChatLogic();
        return *this;
    }
    ChatBotModel(ChatBotModel &&o) noexcept
        : image(std::move(o.image)), currentNode(o.currentNode), chatLogic(o.chatLogic) {
        registerWithChatLogic();
        o.currentNode = nullptr;
        o.chatLogic = nullptr;
    }
    ChatBotModel &operator=(ChatBotModel &&o) noexcept {
        if (this == &o) return *this;
        image = std::move(o.image);
        currentNode = o.currentNode;
        chatLogic = o.chatLogic;
        registerWithChatLogic();
        o.currentNode = nullptr;
        o.chatLogic = nullptr;
        return *this;
    }

    void setCurrentNode(NodeModel *node) {
        if (node) currentNode = node;
    }
    void registerWithChatLogic();
};

struct NodeModel {
    int id;
    ChatBotModel chatBot;

    explicit NodeModel(int i) : id(i) {}

    void moveChatbotHere(ChatBotModel bot) {
        std::swap(chatBot, bot);
        chatBot.setCurrentNode(this);
    }
    void moveChatbotToNewNode(NodeModel *newNode) {
        if (newNode) newNode->moveChatbotHere(std::move(chatBot));
    }
};

struct ChatLogicModel {
    std::vector<std::unique_ptr<NodeModel>> nodes;
    ChatBotModel *chatBot;

    ChatLogicModel() : chatBot(nullptr) {}
    ~ChatLogicModel() {}

    NodeModel *addNode(int id) {
        nodes.emplace_back(std::make_unique<NodeModel>(id));
        return nodes.back().get();
    }
    void setChatbotHandle(ChatBotModel *bot) { chatBot = bot; }
};

void ChatBotModel::registerWithChatLogic() {
    if (chatLogic) chatLogic->setChatbotHandle(this);
}

static void test_chatbot_registers_with_chatlogic_on_attach() {
    ChatLogicModel logic;
    NodeModel *root = logic.addNode(1);

    ChatBotModel bot("avatar.png");
    bot.chatLogic = &logic;
    root->moveChatbotHere(std::move(bot));

    assert(logic.chatBot == &root->chatBot);
    assert(root->chatBot.chatLogic == &logic);
    assert(root->chatBot.currentNode == root);
    assert(std::strcmp(root->chatBot.image.data, "avatar.png") == 0);
    std::cout << "  PASS: attaching a bot registers it with its ChatLogic\n";
}

static void test_chatlogic_pointer_rebinds_on_transfer() {
    ChatLogicModel logic;
    NodeModel *n1 = logic.addNode(1);
    NodeModel *n2 = logic.addNode(2);

    ChatBotModel bot("root.png");
    bot.chatLogic = &logic;
    n1->moveChatbotHere(std::move(bot));
    assert(logic.chatBot == &n1->chatBot);

    n1->moveChatbotToNewNode(n2);

    assert(logic.chatBot == &n2->chatBot);
    assert(n2->chatBot.chatLogic == &logic);
    assert(n2->chatBot.currentNode == n2);
    assert(n1->chatBot.image.data == nullptr);
    std::cout << "  PASS: ChatLogic's bot pointer rebinds to the new node on transfer\n";
}

static void test_chatlogic_pointer_follows_multi_hop_chain() {
    ChatLogicModel logic;
    NodeModel *n1 = logic.addNode(1);
    NodeModel *n2 = logic.addNode(2);
    NodeModel *n3 = logic.addNode(3);

    ChatBotModel bot("chain.png");
    bot.chatLogic = &logic;
    n1->moveChatbotHere(std::move(bot));

    n1->moveChatbotToNewNode(n2);
    n2->moveChatbotToNewNode(n3);

    assert(logic.chatBot == &n3->chatBot);
    assert(n3->chatBot.chatLogic == &logic);
    assert(std::strcmp(n3->chatBot.image.data, "chain.png") == 0);
    assert(n1->chatBot.image.data == nullptr);
    assert(n2->chatBot.image.data == nullptr);
    assert(MockImage::alive == 1);
    std::cout << "  PASS: ChatLogic's bot pointer follows the bot through a multi-hop chain\n";
}

static void test_chatlogic_teardown_destroys_chain_without_leaks() {
    assert(MockImage::alive == 0);
    {
        ChatLogicModel logic;
        NodeModel *n1 = logic.addNode(1);
        NodeModel *n2 = logic.addNode(2);

        ChatBotModel bot("teardown.png");
        bot.chatLogic = &logic;
        n1->moveChatbotHere(std::move(bot));
        n1->moveChatbotToNewNode(n2);

        assert(MockImage::alive == 1);
    }
    assert(MockImage::alive == 0);
    std::cout << "  PASS: destroying ChatLogic tears down the whole chain with no leaked image\n";
}

int main() {
    std::cout << "ChatLogic -> ChatBot -> GraphNode ownership chain integration:\n";
    test_chatbot_registers_with_chatlogic_on_attach();
    test_chatlogic_pointer_rebinds_on_transfer();
    test_chatlogic_pointer_follows_multi_hop_chain();
    test_chatlogic_teardown_destroys_chain_without_leaks();
    std::cout << "All ownership chain integration tests passed.\n";
    return 0;
}
