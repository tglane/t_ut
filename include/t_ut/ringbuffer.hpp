#ifndef CPP_UTILITY_RINGBUFFER_HPP
#define CPP_UTILITY_RINGBUFFER_HPP

#include <algorithm>
#include <optional>
#include <type_traits>
#include <stdexcept>

namespace t_ut {

/// Simple ringbuffer class with a static size
/// Buffer can store up to SIZE - 1 elements
template <typename value_type, size_t buffer_size>
class ringbuffer
{
public:
    using size_type = size_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = pointer;
    using const_iterator = const_pointer;

    ringbuffer() = default;

    ringbuffer(size_type count, const_reference value)
    {
        for(size_type i = 0; i < count; ++i)
        {
            push(value);
        }
    }

    template <typename iterator_type>
    ringbuffer(iterator_type first, iterator_type last)
    {
        while(first != last)
        {
            push(*first++);
        }
    }

    ringbuffer(const ringbuffer& rhs)
    {
        *this = rhs;
    }

    ringbuffer(ringbuffer&& rhs)
    {
        *this = std::move(rhs);
    }

    ringbuffer& operator=(const ringbuffer<value_type, buffer_size>& rhs)
    {
        if(&rhs != this)
        {
            m_read = rhs.m_read;
            m_write = rhs.m_write;

            if constexpr(std::is_trivially_copyable<value_type>())
            {
                std::copy_n(rhs.m_buffer, buffer_size, m_buffer);
            }
            else
            {
                for(size_type i = m_read; i != m_write; i = (i +1) % buffer_size)
                {
                    new(&m_buffer[i]) value_type{reinterpret_cast<const_reference>(rhs.m_buffer[i])};
                }
            }
        }
    }

    ringbuffer& operator=(ringbuffer<value_type, buffer_size>&& rhs)
    {
        if(&rhs != this)
        {
            m_read = rhs.m_read;
            m_write = rhs.m_write;

            if constexpr(std::is_trivially_copyable<value_type>())
            {
                std::copy_n(rhs.m_buffer, buffer_size, m_buffer);
            }
            else
            {
                for(size_type i = m_read; i != m_write; i = (i + 1) % buffer_size)
                {
                    new(&m_buffer[i]) value_type{std::move(reinterpret_cast<reference>(rhs.m_buffer[i]))};
                }
            }
        }
        return *this;
    }

    ~ringbuffer()
    {
        if constexpr(!std::is_trivially_destructible<value_type>())
        {
            while(m_read != m_write)
            {
                reinterpret_cast<reference>(m_buffer[m_read]).~value_type();
                m_read = (m_read + 1) % buffer_size;
            }
        }
    }

    constexpr size_type capacity() const
    {
        return buffer_size - 1;
    }

    size_type size() const
    {
        // Not sure if this is correct
        return (m_write - m_read) % buffer_size;
    }

    bool full() const
    {
        return (m_write + 1) % buffer_size == m_read;
    }

    bool empty() const
    {
        return m_read == m_write;
    }

    void advance(size_type elements = 1)
    {
        m_read = (m_read + elements) % buffer_size;
    }

    void push(const_reference in)
    {
        if(full())
        {
            throw std::bad_alloc{};
        }

        new(&m_buffer[m_write]) value_type(in);
        m_write = (m_write + 1) % buffer_size;
    }

    void push(value_type&& in)
    {
        if(full())
        {
            throw std::bad_alloc{};
        }

        new(&m_buffer[m_write]) value_type{std::move(in)};
        m_write = (m_write + 1) % buffer_size;
    }

    template<typename ... construction_types>
    void emplace_back(construction_types&& ... args)
    {
        if(full())
        {
            throw std::bad_alloc{};
        }

        new(&m_buffer[m_write]) value_type{std::forward<construction_types>(args)...};
        m_write = (m_write + 1) % buffer_size;
    }

    bool push_or_override(const_reference in)
    {
        // Check if we need to update the read position in case of overrun
        if((m_write + 1) % buffer_size == m_read)
        {
            // Overrun
            new(&m_buffer[m_write]) value_type{std::move(in)};
            advance();
            m_write = (m_write + 1) % buffer_size;
            if constexpr(!std::is_trivially_destructible<value_type>())
            {
                reinterpret_cast<reference>(m_buffer[m_read]).~value_type();
            }
            return true;
        }
        else
        {
            // No overrun
            push(in);
            return false;
        }
    }

    template<typename ... construction_types>
    bool emplace_or_override(construction_types&& ... args)
    {
        // Check if we need to update the read position in case of overrun
        if((m_write + 1) % buffer_size == m_read)
        {
            // Overrun
            new(&m_buffer[m_write]) value_type{std::forward<construction_types>(args)...};
            advance();
            m_write = (m_write + 1) % buffer_size;
            if constexpr(!std::is_trivially_destructible<value_type>())
            {
                reinterpret_cast<reference>(m_buffer[m_read]).~value_type();
            }
            return true;
        }
        else
        {
            // No overrun
            emplace(std::forward<construction_types>(args)...);
            return false;
        }
    }

    const_reference peek() const
    {
        return *std::launder(reinterpret_cast<const_pointer>(std::addressof(m_buffer[m_read])));
    }

    reference peek()
    {
        return *std::launder(reinterpret_cast<pointer>(std::addressof(m_buffer[m_read])));
    }

    std::optional<value_type> pop()
    {
        if(empty())
        {
            return std::nullopt;
        }

        value_type back_elem = std::move(*std::launder(reinterpret_cast<pointer>(std::addressof(m_buffer[m_read]))));
        if constexpr(!std::is_trivially_destructible<value_type>())
        {
            reinterpret_cast<pointer>(&m_buffer[m_read])->~value_type();
        }

        advance();
        return back_elem;
    }

    value_type pop_unchecked()
    {
        value_type back_elem = std::move(*std::launder(reinterpret_cast<pointer>(std::addressof(m_buffer[m_read]))));
        if constexpr(!std::is_trivially_destructible<value_type>())
        {
            reinterpret_cast<pointer>(&m_buffer[m_read])->~value_type();
        }

        advance();
        return back_elem;
    }

private:
    // Buffer empty: m_read == m_write
    // Buffer full: m_read == m_write + 1
    // Therefore the buffer can only store SIZE - 1 elements
    size_type m_read = 0;
    size_type m_write = 0;

    std::aligned_storage_t<sizeof(value_type), alignof(value_type)> m_buffer[buffer_size];
};

} // namespace t_ut

#endif
