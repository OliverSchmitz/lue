#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"
#include "lue/framework/serial_route.hpp"


namespace lue {

    template<typename OutputElement, typename Policies, Rank rank>
    PartitionedArray<OutputElement, rank> highest_n(
        Policies const& policies, SerialRoute<rank> const& route, Count const nr_cells);

    template<
        typename OutputElement,
        typename Policies,
        typename ZoneElement,
        typename InputElement,
        Rank rank>
    PartitionedArray<OutputElement, rank> highest_n(
        Policies const& policies,
        PartitionedArray<ZoneElement, rank> const& region,
        PartitionedArray<InputElement, rank> const& array,
        Count const nr_cells);

    template<typename OutputElement, typename Policies, typename InputElement, Rank rank>
    PartitionedArray<OutputElement, rank> highest_n(
        Policies const& policies, PartitionedArray<InputElement, rank> const& array, Count const nr_cells);

}  // namespace lue
