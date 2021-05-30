#ifndef CPP_UTILITY_COMPILE_TIME_MAP_HPP
#define CPP_UTILITY_COMPILE_TIME_MAP_HPP

#include <array>
#include <string_view>

namespace utility
{

template <typename KEY, typename VALUE, size_t SIZE>
class compile_time_map
{
    using key_type = KEY;
    using value_type = VALUE;

public:

    constexpr compile_time_map() = default;

    constexpr compile_time_map(const std::array<std::pair<key_type, value_type>, SIZE>& data)
        : buffer_ {data}
    {}

    constexpr bool insert(const key_type& key, const value_type& value)
    {
        // TODO
        return false;
    }

    constexpr bool insert_or_assign(const key_type& key, const value_type& value)
    {
        // TODO
        return false;
    }

    constexpr value_type at(const key_type& key) const
    {
        for(const auto& pair : buffer_)
        {
            if(pair.first == key)
                return pair.second;
        }
        return {};
    }

private:

    std::array<std::pair<key_type, value_type>, SIZE> buffer_;

};

} // namespace utility

#endif
