#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"
#include "lue/framework.hpp"


namespace lue {

    using CellClass = SmallestUnsignedIntegralElement;
    static constexpr CellClass intra_partition_stream_cell = 11;
    static constexpr CellClass ridge_cell = 12;

    static constexpr CellClass inter_partition_stream_cell = 21;
    static constexpr CellClass partition_input_cell = 13;
    static constexpr CellClass partition_output_cell = 14;
    static constexpr CellClass undefined_cell = 55;

    using PartitionClass = SmallestUnsignedIntegralElement;
    static constexpr PartitionClass ready = 31;  // Partition is ready
    static constexpr PartitionClass hot = 32;    // Partition is (partly) being solved
    static constexpr PartitionClass cold = 33;   // Partition is not used


    namespace policy::accu_info {

        template<typename FlowDirectionElement>
        using DefaultPoliciesBase = Policies<
            AllValuesWithinDomain<FlowDirectionElement>,
            OutputsPolicies<
                OutputPolicies<DontMarkNoData<CellClass>, AllValuesWithinRange<CellClass>>,
                OutputPolicies<DontMarkNoData<PartitionClass>, AllValuesWithinRange<PartitionClass>>,
                OutputPolicies<
                    DontMarkNoData<SmallestFloatingPointElement>,
                    AllValuesWithinRange<SmallestFloatingPointElement>>,
                OutputPolicies<DontMarkNoData<CountElement>, AllValuesWithinRange<CountElement>>>,
            InputsPolicies<SpatialOperationInputPolicies<
                SkipNoData<FlowDirectionElement>,
                FlowDirectionHalo<FlowDirectionElement>>>>;


        template<typename FlowDirectionElement>
        class DefaultPolicies: public DefaultPoliciesBase<FlowDirectionElement>
        {

            public:

                using PoliciesBase = DefaultPoliciesBase<FlowDirectionElement>;
                using CellClassOutputPolicies = OutputPoliciesT<PoliciesBase, 0>;
                using PartitionClassOutputPolicies = OutputPoliciesT<PoliciesBase, 1>;
                using SolvableFractionOutputPolicies = OutputPoliciesT<PoliciesBase, 2>;
                using NrInputCellsToSolveOutputPolicies = OutputPoliciesT<PoliciesBase, 3>;
                using FlowDirectionInputPolicies = InputPoliciesT<PoliciesBase, 0>;


                DefaultPolicies():

                    PoliciesBase{
                        DomainPolicyT<PoliciesBase>{},
                        CellClassOutputPolicies{},
                        PartitionClassOutputPolicies{},
                        SolvableFractionOutputPolicies{},
                        NrInputCellsToSolveOutputPolicies{},
                        FlowDirectionInputPolicies{}}

                {
                }
        };


        template<typename FlowDirectionElement>
        using DefaultValuePoliciesBase = Policies<
            AllValuesWithinDomain<FlowDirectionElement>,
            OutputsPolicies<
                OutputPolicies<DefaultOutputNoDataPolicy<CellClass>, AllValuesWithinRange<CellClass>>,
                OutputPolicies<
                    DefaultOutputNoDataPolicy<PartitionClass>,
                    AllValuesWithinRange<PartitionClass>>,
                OutputPolicies<
                    DefaultOutputNoDataPolicy<SmallestFloatingPointElement>,
                    AllValuesWithinRange<SmallestFloatingPointElement>>,
                OutputPolicies<DefaultOutputNoDataPolicy<CountElement>, AllValuesWithinRange<CountElement>>>,
            InputsPolicies<SpatialOperationInputPolicies<
                DetectNoDataByValue<FlowDirectionElement>,
                FlowDirectionHalo<FlowDirectionElement>>>>;


        template<typename FlowDirectionElement>
        class DefaultValuePolicies: public DefaultValuePoliciesBase<FlowDirectionElement>
        {

            public:

                using PoliciesBase = DefaultValuePoliciesBase<FlowDirectionElement>;
                using CellClassOutputPolicies = OutputPoliciesT<PoliciesBase, 0>;
                using PartitionClassOutputPolicies = OutputPoliciesT<PoliciesBase, 1>;
                using SolvableFractionOutputPolicies = OutputPoliciesT<PoliciesBase, 2>;
                using NrInputCellsToSolveOutputPolicies = OutputPoliciesT<PoliciesBase, 3>;
                using FlowDirectionInputPolicies = InputPoliciesT<PoliciesBase, 0>;


                DefaultValuePolicies():

                    PoliciesBase{
                        DomainPolicyT<PoliciesBase>{},
                        CellClassOutputPolicies{},
                        PartitionClassOutputPolicies{},
                        SolvableFractionOutputPolicies{},
                        NrInputCellsToSolveOutputPolicies{},
                        FlowDirectionInputPolicies{}}

                {
                }
        };

    }  // namespace policy::accu_info


    template<typename Policies, typename FlowDirectionElement, Rank rank>
    auto accu_info(
        Policies const& policies, PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
        -> std::tuple<
            PartitionedArray<CellClass, rank>,
            hpx::future<std::vector<PartitionedArray<PartitionClass, rank>>>,
            hpx::future<std::vector<PartitionedArray<SmallestFloatingPointElement, rank>>>,
            hpx::future<std::vector<PartitionedArray<CountElement, rank>>>>;


    template<typename FlowDirectionElement, Rank rank>
    auto accu_info(PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
        -> std::tuple<
            PartitionedArray<CellClass, rank>,
            hpx::future<std::vector<PartitionedArray<PartitionClass, rank>>>,
            hpx::future<std::vector<PartitionedArray<SmallestFloatingPointElement, rank>>>,
            hpx::future<std::vector<PartitionedArray<CountElement, rank>>>>
    {
        using Policies = policy::accu_info::DefaultPolicies<FlowDirectionElement>;

        return accu_info(Policies{}, flow_direction);
    }

}  // namespace lue
