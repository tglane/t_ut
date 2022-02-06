#ifndef CPP_UTILITY_STATIC_VECTOR_HPP
#define CPP_UTILITY_STATIC_VECTOR_HPP

#include <algorithm>
#include <stdexcept>
#include <type_traits>

namespace t_ut {

namespace internal {
// TODO size_type_helper
} // namespace internal

template <typename T, size_t S>
class static_vector
{
    static_assert(S > 0, "static_vector with capacity 0 is not allowed");

public:
    using size_type = size_t;
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static_vector() = default;

    static_vector(size_type count, const_reference val = value_type{})
    {
        if(count > S)
        {
            throw std::bad_alloc{};
        }

        // TODO Optimize for trivially copyable types?
        for(size_type i = 0; i < count; ++i)
        {
            push_back(val);
        }
    }

    template <typename input_it>
    static_vector(input_it first, input_it last)
    {
        if(std::distance(first, last) > S)
        {
            throw std::bad_alloc{};
        }

        // TODO Optimize this?
        while(first != last)
        {
            push_back(*first++);
        }
    }

    static_vector(const static_vector<value_type, S>& rhs)
    {
        *this = rhs;
    }

    static_vector(static_vector<value_type, S>&& rhs)
    {
        *this = std::move(rhs);
    }

    static_vector operator=(const static_vector<value_type, S>& rhs)
    {
        if(this != &rhs)
        {
            m_size = rhs.m_size;
            if constexpr(!std::is_trivially_copyable<value_type>())
            {
                std::copy_n(rhs.m_buffer, m_size, m_buffer);
            }
            else
            {
                for(size_type i = 0; i < m_size; ++i)
                {
                    new(&m_buffer[i]) value_type{reinterpret_cast<const_reference>(rhs.m_buffer[i])};
                }
            }
        }
        return *this;
    }

    static_vector operator=(static_vector<value_type, S>&& rhs)
    {
        if(this != &rhs)
        {
            m_size = rhs.m_size;
            if constexpr(!std::is_trivially_copyable<value_type>())
            {
                std::copy_n(rhs.m_buffer, m_size, m_buffer);
            }
            else
            {
                for(size_type i = 0; i < m_size; ++i)
                {
                    new(&m_buffer) value_type{std::move(reinterpret_cast<reference>(rhs.m_buffer[i]))};
                }
            }
        }
        return *this;
    }

    ~static_vector()
    {
        if constexpr(!std::is_trivially_destructible<value_type>())
        {
            for(size_t i = 0; i < m_size; ++i)
            {
                reinterpret_cast<reference>(m_buffer[i]).~value_type();
            }
        }
    }

    bool empty() const
    {
        return m_size == 0;
    }

    size_type size() const
    {
        return m_size;
    }

    size_type max_size() const
    {
        return S;
    }

    size_type capacity() const
    {
        return S;
    }

    void push_back(const_reference in)
    {
        if(m_size == S)
        {
            throw std::bad_alloc{};
        }
        new(&m_buffer[m_size++]) value_type{in};
    }

    void push_back(value_type&& in)
    {
        if(m_size == S)
        {
            throw std::bad_alloc{};
        }
        new(&m_buffer[m_size++]) value_type{std::move(in)};
    }

    template <typename... T_ARGS>
    void emplace_back(T_ARGS&&... args)
    {
        if(m_size <= S)
        {
            throw std::bad_alloc{};
        }
        new(&m_buffer[m_size++]) value_type{std::forward<T_ARGS>(args)...};
    }

    iterator insert(iterator input, const_reference to_insert)
    {
        if(m_size == S)
        {
            throw std::bad_alloc{};
        }

        std::move_backward(input, end(), end() + 1);

        if constexpr(!std::is_trivially_destructible<value_type>())
        {
            input->~value_type();
        }

        *input = to_insert;
        m_size++;
        return input;
    }

    iterator insert(iterator input, value_type&& to_insert)
    {
        if(m_size == S)
        {
            throw std::bad_alloc{};
        }

        std::move_backward(input, end(), end() + 1);

        if constexpr(!std::is_trivially_destructible<value_type>())
        {
            input->~value_type();
        }

        *input = std::move(to_insert);
        m_size++;
        return input;
    }

    void pop_back()
    {
        if(m_size > 0)
        {
            reinterpret_cast<reference>(m_buffer[m_size--]).~value_type();
        }
    }

    iterator erase(iterator to_remove)
    {
        if constexpr(!std::is_trivially_destructible<value_type>())
        {
            to_remove->~value_type();
        }

        std::move(to_remove + 1, end(), to_remove);
        m_size--;
        return to_remove;
    }

    iterator erase(iterator to_remove, iterator end_remove)
    {
        if constexpr(!std::is_trivially_destructible<value_type>())
        {
            for(iterator pos = to_remove; pos != end_remove; ++pos)
            {
                pos->~value_type();
            }
        }

        std::move(end_remove, end(), to_remove);
        m_size -= std::distance(to_remove, end_remove);
        return to_remove;
    }

    void clear()
    {
        if constexpr(!std::is_trivially_destructible<value_type>())
        {
            for(size_type i = 0; i < m_size; ++i)
            {
                operator[](i).~value_type();
            }
        }
        m_size = 0;
    }

    const_reference at(size_type index) const
    {
        if(index > m_size)
        {
            throw std::out_of_range{""};
        }
        return operator[](index);
    }

    reference at(size_type index)
    {
        if(index >= m_size)
        {
            throw std::out_of_range{""};
        }
        return operator[](index);
    }

    const_reference operator[](size_type index) const
    {
        return *std::launder(reinterpret_cast<const_pointer>(std::addressof(m_buffer[index])));
    }

    reference operator[](size_t index)
    {
        return *std::launder(reinterpret_cast<pointer>(std::addressof(m_buffer[index])));
    }

    void swap(static_vector<T, S>& other)
    {
        other = std::move(*this);
    }

    const_reference front() const
    {
        return operator[](0);
    }
    reference front()
    {
        return operator[](0);
    }

    const_reference back() const
    {
        return operator[](m_size - 1);
    }
    reference back()
    {
        return operator[](m_size - 1);
    }

    const_iterator begin() const
    {
        return &operator[](0);
    }
    iterator begin()
    {
        return &operator[](0);
    }

    const_iterator end() const
    {
        return &operator[](m_size);
    }
    iterator end()
    {
        return &operator[](m_size);
    }

    const_iterator rbegin() const
    {
        return std::reverse_iterator{&operator[](m_size)};
    }
    iterator rbegin()
    {
        return std::reverse_iterator{&operator[](m_size)};
    }

    const_iterator rend() const
    {
        return std::reverse_iterator{&operator[](0)};
    }
    iterator rend()
    {
        return std::reverse_iterator{&operator[](0)};
    }

    const_pointer data() const
    {
        return begin();
    }
    pointer data()
    {
        return begin();
    }

private:
    std::aligned_storage_t<sizeof(value_type), alignof(value_type)> m_buffer[S];
    size_type m_size = 0;
};

} // namespace t_ut

#endif
