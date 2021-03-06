#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/ternary_local_operation.hpp"


namespace lue {
namespace detail {

template<
    typename ConditionElement,
    typename InputElement>
class Where
{

public:

    using OutputElement = InputElement;

    constexpr OutputElement operator()(
        ConditionElement const condition,
        InputElement const true_value,
        InputElement const false_value) const noexcept
    {
        return condition ? true_value : false_value;
    }

};

}  // namespace detail


/*!
    @brief      Conditionally assign elements
    @param      condition Condition to use for selecting elements from @a
                true_array or @a false_value
    @param      true_array Value to use for those elements for which @a
                condition evaluates to true
    @param      false_value Value to use for those elements for which @a
                condition evaluates to false
*/
template<
    typename ConditionElement,
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> where(
    PartitionedArray<ConditionElement, rank> const& condition,
    PartitionedArray<Element, rank> const& true_array,
    hpx::shared_future<Element> const false_value)
{
    return ternary_local_operation(
            condition, true_array, false_value,
            detail::Where<ConditionElement, Element>{});
}


/*!
    @overload
*/
template<
    typename ConditionElement,
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> where(
    PartitionedArray<ConditionElement, rank> const& condition,
    PartitionedArray<Element, rank> const& true_array,
    Element const false_value)
{
    return where(
        condition, true_array,
        hpx::make_ready_future<Element>(false_value).share());
}

}  // namespace lue
