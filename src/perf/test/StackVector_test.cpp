#include <perf/StackVector.h>

#include <gtest/gtest.h>

namespace perf
{
    TEST(StackVector_test, new_vector_has_zero_size)
    {
        auto v = StackVector<int, 10>{};
        EXPECT_EQ(0, v.size());
    }

    TEST(StackVector_test, pushed_elements_are_accessible)
    {
        auto v = StackVector<int, 10>{};
        v.push_back(1);
        v.push_back(2);
        v.push_back(3);
        EXPECT_EQ(1, v[0]);
        EXPECT_EQ(2, v[1]);
        EXPECT_EQ(3, v[2]);
    }

    TEST(StackVector_test, can_iterate_elements)
    {
        auto v = StackVector<int, 10>{};
        v.push_back(1);
        v.push_back(2);
        v.push_back(3);

        std::vector<int> actual;
        std::vector<int> expected{1,2,3};

        for (auto const& e : v)
        {
            actual.push_back(e);
        }

        EXPECT_EQ(expected, actual);
    }

    TEST(StackVector_test, can_store_none_default_constructable_types)
    {
        struct Thing
        {
            explicit Thing(int x) {}
        };

        auto v = StackVector<Thing, 10>{};
        v.push_back(Thing{1});
    }

    TEST(StackVector_test, throws_if_exceed_capacity)
    {
        auto v = StackVector<int, 1>{};
        v.push_back(1);
        EXPECT_THROW(v.push_back(2), std::logic_error);
    }
}
