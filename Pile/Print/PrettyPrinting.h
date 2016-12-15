#pragma once

#include <ostream>

namespace Pile
{
    template<size_t Size, typename... Args>
    typename std::enable_if<Size == 0, void>::type tuple_print_hlp(std::ostream&, const std::tuple<Args...>&)
    {}

    template<size_t Size, typename... Args>
    typename std::enable_if<(Size > 0), void>::type tuple_print_hlp(std::ostream& ostr, const std::tuple<Args...>& t)
    {
        tuple_print_hlp<Size - 1>(ostr, t);
        ostr << get<Size - 1>(t) << " ";
    }

    template<size_t Size, typename... Args>
    typename std::enable_if<Size == 0, void>::type tuple_print(std::ostream& ostr, const std::tuple<Args...>& t)
    {}

    template<size_t Size, typename... Args>
    typename std::enable_if<(Size > 0), void>::type tuple_print(std::ostream& ostr, const std::tuple<Args...>& t)
    {
        tuple_print_hlp<Size - 1>(ostr, t);
        ostr << get<Size - 1>(t);
    }

    template<typename... Args>
    std::ostream& operator<<(std::ostream& ostr, const std::tuple<Args...>& t)
    {
        tuple_print<tuple_size<tuple<Args...> >::value>(ostr, t);
        return ostr;
    }
}
