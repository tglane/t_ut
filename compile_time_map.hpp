#ifndef CPP_UTILITY_COMPILE_TIME_MAP_HPP
#define CPP_UTILITY_COMPILE_TIME_MAP_HPP

#include <array>

namespace utility
{

template <typename KEY, typename VALUE, std::size_t SIZE>
class compile_time_map
{
    using key_type = KEY;
    using value_type = VALUE;

public:

    template <typename ... PAIR>
    constexpr compile_time_map(PAIR&& ... pairs)
        : m_buffer {static_cast<PAIR &&>(pairs) ...}
    {}

    constexpr compile_time_map(const std::array<std::pair<key_type, value_type>, SIZE> &data)
        : m_buffer {data}
    {}

    constexpr value_type operator[](const key_type& key) const
    {
        for(const auto& pair : m_buffer)
        {
            if(pair.first == key)
                return pair.second;
        }
        return {};
    }

    constexpr value_type at(const key_type& key) const
    {
        for(const auto& pair : m_buffer)
        {
            if(pair.first == key)
                return pair.second;
        }
        return {};
    }

private:

    std::array<std::pair<key_type, value_type>, SIZE> m_buffer;

};

} // namespace utility

#endif
