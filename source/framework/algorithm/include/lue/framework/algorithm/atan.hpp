#pragma once
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include <cmath>


namespace lue {
namespace detail {

template<
    typename InputElement>
class ATan
{

public:

    static_assert(std::is_floating_point_v<InputElement>);

    using OutputElement = InputElement;

    OutputElement operator()(
        InputElement const& input_element) const noexcept
    {
        return std::atan(input_element);
    }

};

}  // namespace detail


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> atan(
    PartitionedArray<Element, rank> const& array)
{
    return unary_local_operation(array, detail::ATan<Element>{});
}

}  // namespace lue
