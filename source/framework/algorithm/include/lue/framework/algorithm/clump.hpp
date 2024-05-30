#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    template<typename Policies, Rank rank>
    auto clump(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& zone) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;

}  // namespace lue
