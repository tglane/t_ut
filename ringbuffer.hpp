#ifndef CPP_UTILITY_RINGBUFFER_HPP
#define CPP_UTILITY_RINGBUFFER_HPP

#include <optional>

namespace utility {

/// Simple ringbuffer class with a static size
/// Buffer can store up to SIZE - 1 elements
template<typename T, size_t SIZE>
class ringbuffer
{
    using stored_type = T;

public:

    constexpr size_t size() const
    {
        return SIZE;
    }

    size_t elements() const
    {
        // Not sure if this is correct
        return (m_write - m_read) % SIZE;
    }

    bool full() const
    {
        return (m_write + 1) % SIZE == m_read;
    }

    bool empty() const
    {
        return m_read == m_write;
    }

    bool insert(const stored_type& in)
    {
        if((m_write + 1) % SIZE == m_read)
            return false;

        m_buffer[m_write] = std::move(in);
        m_write = (m_write + 1) % SIZE;
        return true;
    }

    template<typename ... ARGS>
    bool emplace(ARGS&& ... args)
    {
        if((m_write + 1) % SIZE == m_read)
            return false;

        m_buffer[m_write] = stored_type {std::forward<ARGS>(args)...};
        m_write = (m_write + 1) % SIZE;
        return true;
    }

    bool insert_or_override(const stored_type& in)
    {
        m_buffer[m_write] = std::move(in);
        m_write = (m_write + 1) % SIZE;

        // Check if we need to update the read position in case of overrun
        if(m_write == m_read)
        {
            // Overrun
            m_read = (m_read + 1) % SIZE;
            return true;
        }
        else
        {
            // No overrun
            return false;
        }
    }

    template<typename ... ARGS>
    bool emplace_or_override(ARGS&& ... args)
    {
        m_buffer[m_write] = stored_type {std::forward<ARGS>(args)...};
        m_write = (m_write + 1) % SIZE;

        // Check if we need to update the read position in case of overrun
        if(m_write == m_read)
        {
            // Overrun
            m_read = (m_read + 1) % SIZE;
            return true;
        }
        else
        {
            // No overrun
            return false;
        }
    }

    bool skip(size_t elements = 1)
    {
        if(m_read == m_write)
            return false;

        m_read = (m_read + elements) % SIZE;
        return true;
    }

    const stored_type& read() const
    {
        return m_buffer[m_read];
    }

    std::optional<stored_type> get()
    {
        if(m_read == m_write)
            return std::nullopt;

        stored_type& tmp = m_buffer[m_read];
        m_read = (m_read + 1) % SIZE;
        return tmp;
    }

private:

    // Buffer empty: m_read == m_write
    // Buffer full: m_read == m_write + 1
    // Therefore the buffer can only store SIZE - 1 elements
    size_t m_read = 0;
    size_t m_write = 0;

    stored_type m_buffer[SIZE] = {};

};

} // namespace utility

#endif
