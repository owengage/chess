#pragma once

#include <utility>
#include <vector>

namespace chess
{
    template<typename T>
    struct Tree
    {
        explicit Tree(T root) : m_root{std::move(root)}
        {

        }

        T const& value() const
        {
            return m_root;
        }

        T & value()
        {
            return m_root;
        }

        Tree<T> & add_child(T child)
        {
            m_children.emplace_back(std::move(child));
            return m_children.back();
        }

        std::vector<Tree> const& children() const
        {
            return m_children;
        }

        std::vector<Tree> & children()
        {
            return m_children;
        }

    private:
        T m_root;
        std::vector<Tree<T>> m_children;
    };
}