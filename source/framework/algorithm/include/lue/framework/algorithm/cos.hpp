#pragma once
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include <cmath>


namespace lue {
namespace detail {

template<
    typename InputElement>
class Cos
{

public:

    static_assert(std::is_floating_point_v<InputElement>);

    using OutputElement = InputElement;

    OutputElement operator()(
        InputElement const& input_element) const noexcept
    {
        return std::cos(input_element);
    }

};

}  // namespace detail


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> cos(
    PartitionedArray<Element, rank> const& array)
{
    return unary_local_operation(array, detail::Cos<Element>{});
}

}  // namespace lue
