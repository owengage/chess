#pragma once

#include <type_traits>
#include <utility>
#include <stdexcept>

namespace perf
{
    /*
     * Minimal implementation of a vector type that lives on the stack.
     */
    template<typename T, std::size_t max_capacity>
    struct StackVector
    {
        static_assert(std::is_trivially_destructible<T>::value);

        using iterator = T *;
        using const_iterator = T const *;

        constexpr std::size_t size() const
        {
            return m_size;
        }

        constexpr void push_back(T element)
        {
            if (m_size == max_capacity) { throw std::logic_error{"Exceeded size"}; }
            auto point = &m_data[m_size];
            new(point) T{std::move(element)};
            ++m_size;
        }

        constexpr T& operator[](std::size_t index)
        {
            return reinterpret_cast<T&>(m_data[index]);
        }

        constexpr T& operator[](std::size_t index) const
        {
            return m_data[index];
        }

        constexpr iterator begin()
        {
            return reinterpret_cast<T*>(&m_data[0]);
        }

        constexpr const_iterator begin() const
        {
            return reinterpret_cast<T*>(&m_data[0]);
        }

        constexpr iterator end()
        {
            return reinterpret_cast<T*>(&m_data[0] + m_size);
        }

        constexpr const_iterator end() const
        {
            return reinterpret_cast<T*>(&m_data[0] + m_size);
        }

    private:
        using store = typename std::aligned_storage<sizeof(T), alignof(T)>::type[max_capacity];
        store m_data;
        std::size_t m_size = 0;
    };
}