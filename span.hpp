#ifndef CPP_UTILITY_SPAN_HPP
#define CPP_UTILITY_SPAN_HPP

namespace utility
{

/// Generic non-owning buffer type inspired by golangs slices
/// Used as a generic buffer class to send data from and receive data to
template<typename T>
class span
{
public:

    span() = delete;
    span(const span&) noexcept = default;
    span& operator=(const span&) noexcept = default;
    span(span&&) noexcept = default;
    span& operator=(span&&) noexcept = default;
    ~span() noexcept = default;

    span(T* start, size_t length) noexcept
        : m_start {start}, m_size {length}
    {}

    span(T* start, T* end) noexcept
        : m_start {start}, m_size {static_cast<size_t>(std::distance(start, end) + 1)}
    {}

    template<size_t S>
    explicit span(T (&buffer)[S]) noexcept
        : m_start {buffer}, m_size {S}
    {}

    template<typename ITER>
    span(ITER start, ITER end) noexcept
        : m_start {&(*start)}, m_size {static_cast<size_t>(std::distance(&(*start), &(*end)))}
    {}

    template<typename CONTAINER>
    explicit span(CONTAINER&& con) noexcept
        : m_start {con.data()}, m_size {con.size()}
    {}

    constexpr T* get() const { return m_start; }
    constexpr T* data() const { return m_start; }

    constexpr size_t size() const { return m_size; }

    constexpr bool empty() const { return m_size > 0; }

    constexpr T& operator[](size_t index) { return m_start[index]; }
    constexpr const T& operator[](size_t index) const { return m_start[index]; }

    constexpr T* begin() const { return m_start; }
    constexpr T* end() const { return &(m_start[m_size]); }

    constexpr T& front() const { return m_start[0]; }
    constexpr T& back() const { return m_start[m_size - 1]; }

private:

    T* m_start;
    size_t m_size;

};

/// Template deduction guides for class span
template<typename ITERATOR>
span(ITERATOR, ITERATOR) -> span<typename std::iterator_traits<ITERATOR>::value_type>;

template<typename CONTAINER_TYPE>
span(const CONTAINER_TYPE&) -> span<typename std::remove_reference<decltype(std::declval<CONTAINER_TYPE>().front())>::type>;

} // namespace utility

#endif
