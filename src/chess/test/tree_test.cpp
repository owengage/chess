#include <chess/Tree.h>

#include <gtest/gtest.h>

namespace chess
{
    TEST(tree_test, given_root_node_should_get_back)
    {
        auto tree = Tree<int>{5};
        EXPECT_EQ(5, tree.value());
    }

    TEST(tree_test, given_child_should_get_back)
    {
        auto tree = Tree<int>{1};
        tree.add_child(2);
        auto children = tree.children();
        ASSERT_EQ(1, children.size());
        EXPECT_EQ(2, children[0].value());
    }

    TEST(tree_test, should_be_able_to_add_grandchildren_via_add_child_return_value)
    {
        auto tree = Tree<int>{1};
        auto & child = tree.add_child(2);
        child.add_child(3);

        ASSERT_EQ(1, tree.children().size());
        ASSERT_EQ(1, tree.children()[0].children().size());
        EXPECT_EQ(3, tree.children()[0].children()[0].value());
    }
}