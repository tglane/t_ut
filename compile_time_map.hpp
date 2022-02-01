#ifndef CPP_UTILITY_COMPILE_TIME_MAP_HPP
#define CPP_UTILITY_COMPILE_TIME_MAP_HPP

#include <array>

namespace t_ut
{

namespace
{

    template <typename T, size_t N, size_t ... Ns>
    constexpr std::array<T, N> make_array_internal(const std::initializer_list<T>& init_list,
        std::index_sequence<Ns ...>)
    {
        return {*(init_list.begin() + Ns) ...};
    }

    template <typename T, size_t N>
    constexpr std::array<T, N> make_array(const std::initializer_list<T>& init_list)
    {
        if(N != init_list.size())
            return {};
        return make_array_internal<T, N>(init_list, std::make_index_sequence<N>());
    }

}

template <typename KEY, typename VALUE, size_t SIZE>
class compile_time_map
{
    using key_type = KEY;
    using value_type = VALUE;

public:

    using pair_type = std::pair<key_type, value_type>;

    template <typename ... PAIR>
    constexpr compile_time_map(PAIR&& ... pairs)
        : m_buffer {static_cast<PAIR&&>(pairs) ...}
    {}

    constexpr compile_time_map(const std::initializer_list<pair_type>& init_list)
        : m_buffer {make_array<pair_type, SIZE>(init_list)}
    {}

    constexpr compile_time_map(const std::array<pair_type, SIZE> &data)
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
    std::array<pair_type, SIZE> m_buffer;
};

} // namespace t_ut

#endif
