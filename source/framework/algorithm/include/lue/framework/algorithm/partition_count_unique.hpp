#pragma once
#include "lue/framework/algorithm/partition_operation.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"
#include <hpx/serialization/map.hpp>
#include <set>


namespace lue {
    namespace detail {

        template<
            typename InputElement,
            typename OutputElement_>
        class CountUnique
        {

            public:

                using OutputElement = OutputElement_;

                CountUnique()=default;

                void add_value(
                    InputElement const& value)
                {
                    _values.insert(value);
                }

                bool result_is_valid() const
                {
                    return !_values.empty();
                }

                OutputElement result() const
                {
                    return _values.size();
                }

            private:

                friend class hpx::serialization::access;

                void serialize(
                    hpx::serialization::input_archive& archive,
                    [[maybe_unused]] unsigned int const version)
                {
                    lue_assert(_values.empty());
                    archive & _values;
                }

                void serialize(
                    hpx::serialization::output_archive& archive,
                    [[maybe_unused]] unsigned int const version) const
                {
                    lue_assert(_values.empty());
                    archive & _values;
                }

                std::set<InputElement> _values;

        };

    }  // namespace detail


    namespace policy::partition_count_unique {

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            OutputElements<OutputElement>,
            InputElements<InputElement>>;


        template<
            typename OutputElement,
            typename InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            OutputElements<OutputElement>,
            InputElements<InputElement>>;

    }  // namespace policy::partition_count_unique


    template<
        typename Policies,
        typename InputElement,
        typename OutputElement=Count,
        Rank rank>
    PartitionedArray<OutputElement, rank> partition_count_unique(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& array)
    {
        using Functor = detail::CountUnique<InputElement, OutputElement>;

        return partition_operation(policies, array, Functor{});
    }


    template<
        typename InputElement,
        typename OutputElement=Count,
        Rank rank>
    PartitionedArray<OutputElement, rank> partition_count_unique(
        PartitionedArray<InputElement, rank> const& array)
    {
        using Policies = policy::partition_count_unique::DefaultPolicies<OutputElement, InputElement>;

        return partition_count_unique(Policies{}, array);
    }

}  // namespace lue
