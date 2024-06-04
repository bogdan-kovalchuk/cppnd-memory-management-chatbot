#include <cassert>
#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>

struct EdgeModel {
    int id;
    static int alive;
    explicit EdgeModel(int i) : id(i) { ++alive; }
    ~EdgeModel() { --alive; }
};
int EdgeModel::alive = 0;

struct NodeModel {
    int id;
    std::vector<std::unique_ptr<EdgeModel>> childEdges;
    static int alive;
    static std::vector<int> destructionOrder;

    explicit NodeModel(int i) : id(i) { ++alive; }
    ~NodeModel() { --alive; destructionOrder.push_back(id); }

    NodeModel(const NodeModel &) = delete;
    NodeModel &operator=(const NodeModel &) = delete;

    void addChildEdge(std::unique_ptr<EdgeModel> edge) {
        childEdges.push_back(std::move(edge));
    }
};
int NodeModel::alive = 0;
std::vector<int> NodeModel::destructionOrder;

// Mirrors ChatLogic: owns nodes via unique_ptr, declares its own destructor,
// and (like the real class) never declares copy or move members.
struct ChatLogicModel {
    std::vector<std::unique_ptr<NodeModel>> nodes;

    ChatLogicModel() {}
    ~ChatLogicModel() {}

    NodeModel *addNode(int id) {
        auto it = std::find_if(nodes.begin(), nodes.end(),
                                [id](const std::unique_ptr<NodeModel> &n) { return n->id == id; });
        if (it != nodes.end())
            return it->get();
        nodes.emplace_back(std::make_unique<NodeModel>(id));
        return nodes.back().get();
    }
};

static void test_chatlogic_default_has_no_nodes() {
    ChatLogicModel logic;
    assert(logic.nodes.empty());
    std::cout << "  PASS: default ChatLogic owns no nodes\n";
}

static void test_chatlogic_add_node_creates_new_entry() {
    ChatLogicModel logic;
    NodeModel *node = logic.addNode(1);
    assert(node != nullptr);
    assert(logic.nodes.size() == 1);
    assert(logic.nodes.front()->id == 1);
    std::cout << "  PASS: addNode creates and owns a new node\n";
}

static void test_chatlogic_add_node_deduplicates_by_id() {
    ChatLogicModel logic;
    NodeModel *first = logic.addNode(7);
    NodeModel *second = logic.addNode(7);
    assert(first == second);
    assert(logic.nodes.size() == 1);
    std::cout << "  PASS: addNode is idempotent for a duplicate ID\n";
}

static void test_chatlogic_owns_multiple_distinct_nodes() {
    ChatLogicModel logic;
    logic.addNode(1);
    logic.addNode(2);
    logic.addNode(3);
    assert(logic.nodes.size() == 3);
    assert(NodeModel::alive == 3);
    std::cout << "  PASS: ChatLogic owns multiple distinct nodes\n";
}

static void test_chatlogic_destructor_destroys_all_owned_nodes() {
    assert(NodeModel::alive == 0);
    {
        ChatLogicModel logic;
        logic.addNode(1);
        logic.addNode(2);
        logic.addNode(3);
        assert(NodeModel::alive == 3);
    }
    assert(NodeModel::alive == 0);
    std::cout << "  PASS: destroying ChatLogic destroys every owned node\n";
}

static void test_chatlogic_teardown_destroys_every_node_exactly_once() {
    NodeModel::destructionOrder.clear();
    {
        ChatLogicModel logic;
        logic.addNode(10);
        logic.addNode(20);
        logic.addNode(30);
    }
    std::vector<int> destroyed = NodeModel::destructionOrder;
    std::sort(destroyed.begin(), destroyed.end());
    assert((destroyed == std::vector<int>{10, 20, 30}));
    std::cout << "  PASS: teardown destroys each owned node exactly once\n";
}

static void test_chatlogic_teardown_cascades_to_owned_edges() {
    assert(EdgeModel::alive == 0);
    {
        ChatLogicModel logic;
        NodeModel *node = logic.addNode(1);
        node->addChildEdge(std::make_unique<EdgeModel>(100));
        node->addChildEdge(std::make_unique<EdgeModel>(200));
        assert(EdgeModel::alive == 2);
    }
    assert(EdgeModel::alive == 0);
    std::cout << "  PASS: destroying ChatLogic cascades to edges owned by its nodes\n";
}

int main() {
    std::cout << "ChatLogic ownership lifecycle:\n";
    test_chatlogic_default_has_no_nodes();
    test_chatlogic_add_node_creates_new_entry();
    test_chatlogic_add_node_deduplicates_by_id();
    test_chatlogic_owns_multiple_distinct_nodes();
    test_chatlogic_destructor_destroys_all_owned_nodes();
    test_chatlogic_teardown_destroys_every_node_exactly_once();
    test_chatlogic_teardown_cascades_to_owned_edges();
    std::cout << "All ChatLogic lifecycle tests passed.\n";
    return 0;
}
