#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    template<typename Policies, Rank rank>
    auto minimum(
        Policies const& policies, PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array)
        -> Scalar<policy::OutputElementT<Policies, 0>>;

}  // namespace lue
