#ifndef CPP_UTILITY_FUNCTION_REF_HPP
#define CPP_UTILITY_FUNCTION_REF_HPP

#include <functional>
#include <utility>

namespace utility
{

/// Wrapper class for generically passing around lambdas or function pointers without
///     taking ownership.
///     Approach to implement my own basic version of upcoming std::function_ref
/// Base function template
template<typename FUNC>
class function_ref;

/// Specialization
template<typename RET, typename ... ARGS>
class function_ref<RET(ARGS...)>
{
    using return_type = RET;

public:

    template<typename FUNC>
    function_ref(FUNC&& func)
        : m_repr {reinterpret_cast<void*>(std::addressof(func))}
    {
        m_callback = +[](void* repr, ARGS&& ... args) -> return_type {
            return std::invoke(
                *reinterpret_cast<typename std::add_pointer<FUNC>::type>(repr),
                static_cast<ARGS&&>(args)...
            );
        };
    }

    return_type operator()(ARGS ... args) const
    {
        return m_callback(m_repr, static_cast<ARGS&&>(args)...);
    }

private:

    void* m_repr;

    return_type (*m_callback)(void*, ARGS&& ...);

};

/// Template deduction guide
template<typename RET, typename ... ARGS>
function_ref(RET (*)(ARGS...)) -> function_ref<RET(ARGS...)>;

} // namespace utility

#endif
