#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"
#include <numeric>


namespace lue {
namespace detail {

template<
    typename Partition>
[[nodiscard]] hpx::future<void> unique_id_partition(
    // TODO non-const ref is not possible for actions. Is this an issue
    //     in case of component client instances?
    Partition partition,
    ElementT<Partition> const start_value)
{
    // - Create a new collection of data elements
    // - Assign unique IDs to the cells
    // - Assign the collection of data elements
    // - Return a future that becomes ready once the data has been
    //     assigned to the existing partition

    using Data = DataT<Partition>;
    using Shape = ShapeT<Partition>;

    // Given the partition's shape, we can create a new collection for
    // partition elements
    return partition.shape().then(
        hpx::util::unwrapping(
            [partition, start_value](
                Shape const& shape) mutable
            {
                TargetIndex const target_idx =
                    hpx::get_ptr<Partition>(partition).get()->target_idx();

                Data data{shape, target_idx};
                std::iota(data.begin(), data.end(), start_value);

                // This runs asynchronous and returns a future<void>
                return partition.set_data(std::move(data)); // data, CopyMode::share);
            }));

    // auto assigned = data.then(
    //     hpx::util::unwrapping(
    //         [start_value](Data& partition_data)
    //         {
    //             std::iota(
    //                 partition_data.begin(), partition_data.end(), start_value);
    //         }));

    // return assigned.then(
    //     hpx::util::unwrapping(
    //         [partition](Data& assigned_data)
    //         {
    //             partition.set_data(assigned_data);
    //         }));
}


template<
    typename Element,
    Rank rank>
struct UniqueIDPartitionAction
{
};

}  // namespace detail


template<
    typename Partition>
struct UniqueIDPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::unique_id_partition<Partition>),
        &detail::unique_id_partition<Partition>,
        UniqueIDPartitionAction<Partition>>
{};


/*!
    @brief      Fill a partitioned array in-place with unique IDs
    @tparam     Element Type of elements in the array
    @tparam     rank Rank of the array
    @param      array Partitioned array
    @return     Future that becomes ready once the algorithm has finished

    The existing @a array passed in is updated. Use the returned future if
    you need to know when the filling is done.
*/
template<
    typename Element,
    Rank rank>
[[nodiscard]] hpx::future<void> unique_id(
    PartitionedArray<Element, rank>& array)
{
    // - Iterate over all partitions
    //     - Determine first value for partition
    //     - Call action that fills the partition with unique IDs
    // - Return future that becomes ready once all partitions are done
    //     assigning values to all cells

    using InputArray = PartitionedArray<Element, rank>;
    using InputPartition = PartitionT<InputArray>;

    auto const nr_partitions = lue::nr_partitions(array);

    std::vector<hpx::future<void>> unique_id_partitions(nr_partitions);

    // FIXME make all of this asynchronous

    UniqueIDPartitionAction<InputPartition> action;

    std::vector<hpx::future<Count>> partition_sizes(nr_partitions);

    {
        // Request the sizes of all partitions and wait until they are
        // available
        for(Index p = 0; p < nr_partitions; ++p) {
            InputPartition& partition = array.partitions()[p];
            partition_sizes[p] = partition.nr_elements();
        }

        hpx::wait_all(partition_sizes);
    }

    Element start_value = 0;

    for(Index p = 0; p < nr_partitions; ++p) {

        unique_id_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [action, start_value](
                InputPartition const& input_partition)
            {
                return action(
                    hpx::get_colocation_id(
                        hpx::launch::sync, input_partition.get_id()),
                    input_partition,
                    start_value);
            },

            array.partitions()[p]);



        // InputPartition& partition = array.partitions()[p];

        // unique_id_partitions[p] =
        //     hpx::get_colocation_id(partition.get_id()).then(
        //         hpx::util::unwrapping(
        //             [=](
        //                 hpx::id_type const locality_id)
        //             {
        //                 return hpx::dataflow(
        //                     hpx::launch::async,
        //                     action,
        //                     locality_id,
        //                     partition,
        //                     start_value);
        //             }
        //         )
        //     );

        start_value += partition_sizes[p].get();
    }

    return hpx::when_all(unique_id_partitions);
}

}  // namespace lue
