#ifndef CPP_UTILITY_RINGBUFFER_HPP
#define CPP_UTILITY_RINGBUFFER_HPP

namespace utility {

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

    bool store(const stored_type& in)
    {
        bool overrun;

        // Check if we need to update the read position in case of overrun
        if(m_write % SIZE == m_read)
        {
            m_read = (m_read + 1) % SIZE;
            overrun = true;
        }
        else
        {
            overrun = false;
        }

        m_buffer[m_write] = in;
        m_write = (m_write + 1) % SIZE;
        return overrun;
    }

    bool skip(size_t elements = 1)
    {
        // TODO return bool to indicate overrun
        m_read = (m_read + elements) % SIZE;
        return false;
    }

    const stored_type& read() const
    {
        // TODO Check if there is content to read
        return m_buffer[m_read];
    }

    stored_type get()
    {
        // TODO Check if there is content to read
        stored_type& tmp = m_buffer[m_read];
        m_read = (m_read + 1) % SIZE;
        return tmp;
    }

private:

    size_t m_read = 0;
    size_t m_write = 0;

    stored_type m_buffer[SIZE];

};

} // namespace utility

#endif
