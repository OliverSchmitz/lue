#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {

// If this is needed elsewhere, put it in algorithm/functor_traits.hpp
// Refactor with binary_operation, focal_operation.hpp
template<
    typename Functor>
using OutputElementT = typename Functor::OutputElement;


namespace detail {

template<
    typename InputPartition,
    typename OutputPartition,
    typename Functor>
OutputPartition unary_local_operation_partition(
    InputPartition const& input_partition,
    Functor functor)
{
    assert(
        hpx::get_colocation_id(input_partition.get_id()).get() ==
        hpx::find_here());

    using InputData = DataT<InputPartition>;

    using OutputData = DataT<OutputPartition>;

    auto const input_partition_server_ptr{hpx::get_ptr(input_partition).get()};
    auto const& input_partition_server{*input_partition_server_ptr};

    auto offset{input_partition_server.offset()};
    InputData input_partition_data{
        input_partition_server.data(CopyMode::share)};
    TargetIndex const target_idx = input_partition_data.target_idx();

    OutputData output_partition_data{input_partition_data.shape(), target_idx};

    std::transform(
        input_partition_data.begin(), input_partition_data.end(),
        output_partition_data.begin(), functor);

    return OutputPartition{
        hpx::find_here(),
        offset,
        std::move(output_partition_data)};
}


template<
    typename InputPartition,
    typename OutputPartition,
    typename Functor>
struct UnaryLocalOperationPartitionAction:
    hpx::actions::make_action<
            decltype(&unary_local_operation_partition<
                InputPartition, OutputPartition, Functor>),
            &unary_local_operation_partition<
                InputPartition, OutputPartition, Functor>,
            UnaryLocalOperationPartitionAction<
                InputPartition, OutputPartition, Functor>
        >
{};

}  // namespace detail


template<
    typename InputElement,
    Rank rank,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> unary_local_operation(
    PartitionedArray<InputElement, rank> const& input_array,
    Functor const& functor)
{
    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;
    using OutputPartition = PartitionT<OutputArray>;

    detail::UnaryLocalOperationPartitionAction<
        InputPartition, OutputPartition, Functor> action;
    OutputPartitions output_partitions{
        shape_in_partitions(input_array), scattered_target_index()};

    for(Index p = 0; p < nr_partitions(input_array); ++p) {

        // This loops will keep AGAS busy for a while, to return the
        // locality IDs the components are located on. This might be why
        // Vampir traces may show threads are starving(?).
        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            // hpx::util::annotated_function(

                [action, functor](
                    InputPartition const& input_partition,
                    hpx::future<hpx::id_type>&& locality_id)
                {
                    return action(
                        locality_id.get(),
                        input_partition,
                        functor);
                },

            //     "spawn_unary_local_operation_partition"),

            input_array.partitions()[p],
            hpx::get_colocation_id(input_array.partitions()[p].get_id()));

    }

    return OutputArray{shape(input_array), std::move(output_partitions)};
}

}  // namespace lue
