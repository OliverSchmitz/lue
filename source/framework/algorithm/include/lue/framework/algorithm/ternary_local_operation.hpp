#pragma once
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {
namespace detail {
namespace ternary_local_operation {

template<
    typename Input1,
    typename Input2,
    typename Input3,
    typename OutputPartition,
    typename Functor>
class OverloadPicker
{
};


template<
    typename InputElement1,
    typename InputElement2,
    typename InputElement3,
    typename OutputPartition,
    typename Functor>
class OverloadPicker<
    ArrayPartition<InputElement1, rank<OutputPartition>>,
    ArrayPartition<InputElement2, rank<OutputPartition>>,
    ArrayPartition<InputElement3, rank<OutputPartition>>,
    OutputPartition,
    Functor>

{

public:

    static constexpr Rank rank{lue::rank<OutputPartition>};

    using InputPartition1 = ArrayPartition<InputElement1, rank>;
    using InputPartition2 = ArrayPartition<InputElement2, rank>;
    using InputPartition3 = ArrayPartition<InputElement3, rank>;

    static OutputPartition ternary_local_operation_partition(
        InputPartition1 const& input_partition1,
        InputPartition2 const& input_partition2,
        InputPartition3 const& input_partition3,
        Functor functor)
    {
        using Offset = OffsetT<InputPartition1>;
        using InputData1 = DataT<InputPartition1>;
        using InputData2 = DataT<InputPartition2>;
        using InputData3 = DataT<InputPartition3>;

        using OutputData = DataT<OutputPartition>;

        lue_assert(input_partition1.is_ready());
        lue_assert(input_partition2.is_ready());
        lue_assert(input_partition3.is_ready());

        return hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [input_partition1, input_partition2, input_partition3, functor](
                        Offset const& offset,
                        InputData1 const& input_data1,
                        InputData2 const& input_data2,
                        InputData3 const& input_data3)
                    {
                        AnnotateFunction annotation{"ternary_local_operation"};

                        HPX_UNUSED(input_partition1);
                        HPX_UNUSED(input_partition2);
                        HPX_UNUSED(input_partition3);

                        lue_assert(input_data2.nr_elements() == input_data1.nr_elements());
                        lue_assert(input_data3.nr_elements() == input_data1.nr_elements());

                        OutputData output_data{input_data1.shape()};
                        Count const nr_elements{lue::nr_elements(input_data1)};

                        for(Index i = 0; i < nr_elements; ++i)
                        {
                            output_data[i] = functor(input_data1[i], input_data2[i], input_data3[i]);
                        }

                        return OutputPartition{hpx::find_here(), offset, std::move(output_data)};
                    }

                ),
            input_partition1.offset(),
            input_partition1.data(),
            input_partition2.data(),
            input_partition3.data());
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&ternary_local_operation_partition),
            &ternary_local_operation_partition,
            Action>
    {};

};


template<
    typename InputElement1,
    typename InputElement2,
    typename InputElement3,
    typename OutputPartition,
    typename Functor>
class OverloadPicker<
    ArrayPartition<InputElement1, rank<OutputPartition>>,
    ArrayPartition<InputElement2, rank<OutputPartition>>,
    InputElement3,
    OutputPartition,
    Functor>

{

public:

    static constexpr Rank rank{lue::rank<OutputPartition>};

    using InputPartition1 = ArrayPartition<InputElement1, rank>;
    using InputPartition2 = ArrayPartition<InputElement2, rank>;

    static OutputPartition ternary_local_operation_partition(
        InputPartition1 const& input_partition1,
        InputPartition2 const& input_partition2,
        InputElement3 const input_scalar,
        Functor functor)
    {
        using Offset = OffsetT<InputPartition1>;
        using InputData1 = DataT<InputPartition1>;
        using InputData2 = DataT<InputPartition2>;

        using OutputData = DataT<OutputPartition>;

        lue_assert(input_partition1.is_ready());
        lue_assert(input_partition2.is_ready());

        return hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [input_partition1, input_partition2, functor, input_scalar](
                        Offset const& offset,
                        InputData1 const& input_data1,
                        InputData2 const& input_data2)
                    {
                        AnnotateFunction annotation{"ternary_local_operation_partition"};

                        HPX_UNUSED(input_partition1);
                        HPX_UNUSED(input_partition2);

                        lue_assert(input_data2.nr_elements() == input_data1.nr_elements());

                        OutputData output_data{input_data1.shape()};
                        Count const nr_elements{lue::nr_elements(input_data1)};

                        for(Index i = 0; i < nr_elements; ++i)
                        {
                            output_data[i] = functor(input_data1[i], input_data2[i], input_scalar);
                        }

                        return OutputPartition{hpx::find_here(), offset, std::move(output_data)};
                    }

                ),
            input_partition1.offset(),
            input_partition1.data(),
            input_partition2.data());
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&ternary_local_operation_partition),
            &ternary_local_operation_partition,
            Action>
    {};

};


template<
    typename InputElement1,
    typename InputElement2,
    typename InputElement3,
    typename OutputPartition,
    typename Functor>
class OverloadPicker<
    ArrayPartition<InputElement1, rank<OutputPartition>>,
    InputElement2,
    ArrayPartition<InputElement3, rank<OutputPartition>>,
    OutputPartition,
    Functor>

{

public:

    static constexpr Rank rank{lue::rank<OutputPartition>};

    using InputPartition1 = ArrayPartition<InputElement1, rank>;
    using InputPartition2 = ArrayPartition<InputElement3, rank>;

    static OutputPartition ternary_local_operation_partition(
        InputPartition1 const& input_partition1,
        InputElement2 const input_scalar,
        InputPartition2 const& input_partition2,
        Functor functor)
    {
        using Offset = OffsetT<InputPartition1>;
        using InputData1 = DataT<InputPartition1>;
        using InputData2 = DataT<InputPartition2>;

        using OutputData = DataT<OutputPartition>;

        lue_assert(input_partition1.is_ready());
        lue_assert(input_partition2.is_ready());

        return hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [input_partition1, input_partition2, functor, input_scalar](
                        Offset const& offset,
                        InputData1 const& input_data1,
                        InputData2 const& input_data2)
                    {
                        AnnotateFunction annotation{"ternary_local_operation_partition"};

                        HPX_UNUSED(input_partition1);
                        HPX_UNUSED(input_partition2);

                        lue_assert(input_data2.nr_elements() == input_data1.nr_elements());

                        OutputData output_data{input_data1.shape()};
                        Count const nr_elements{lue::nr_elements(input_data1)};

                        for(Index i = 0; i < nr_elements; ++i)
                        {
                            output_data[i] = functor(input_data1[i], input_scalar, input_data2[i]);
                        }

                        return OutputPartition{hpx::find_here(), offset, std::move(output_data)};
                    }

                ),
            input_partition1.offset(),
            input_partition1.data(),
            input_partition2.data());
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&ternary_local_operation_partition),
            &ternary_local_operation_partition,
            Action>
    {};

};


template<
    typename InputElement1,
    typename InputElement2,
    typename InputElement3,
    typename OutputPartition,
    typename Functor>
class OverloadPicker<
    ArrayPartition<InputElement1, rank<OutputPartition>>,
    InputElement2,
    InputElement3,
    OutputPartition,
    Functor>

{

public:

    static constexpr Rank rank{lue::rank<OutputPartition>};

    using InputPartition = ArrayPartition<InputElement1, rank>;

    static OutputPartition ternary_local_operation_partition(
        InputPartition const& input_partition,
        InputElement2 const input_scalar1,
        InputElement3 const input_scalar2,
        Functor functor)
    {
        using Offset = OffsetT<InputPartition>;
        using InputData = DataT<InputPartition>;

        using OutputData = DataT<OutputPartition>;

        lue_assert(input_partition.is_ready());

        return hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [input_partition, functor, input_scalar1, input_scalar2](
                        Offset const& offset,
                        InputData const& input_data)
                    {
                        AnnotateFunction annotation{"ternary_local_operation_partition"};

                        HPX_UNUSED(input_partition);

                        OutputData output_data{input_data.shape()};
                        Count const nr_elements{lue::nr_elements(input_data)};

                        for(Index i = 0; i < nr_elements; ++i)
                        {
                            output_data[i] = functor(input_data[i], input_scalar1, input_scalar2);
                        }

                        return OutputPartition{hpx::find_here(), offset, std::move(output_data)};
                    }

                ),
            input_partition.offset(),
            input_partition.data());
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&ternary_local_operation_partition),
            &ternary_local_operation_partition,
            Action>
    {};

};

}  // namespace ternary_local_operation


template<
    typename T1,
    typename T2,
    typename T3,
    typename OutputPartition,
    typename Functor>
using TernaryLocalOperationPartitionAction =
    typename ternary_local_operation::OverloadPicker<
        T1, T2, T3, OutputPartition, Functor>::Action;

}  // namespace detail


// local_operation(array, array, array)
template<
    typename InputElement1,
    typename InputElement2,
    typename InputElement3,
    Rank rank,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> ternary_local_operation(
    PartitionedArray<InputElement1, rank> const& input_array1,
    PartitionedArray<InputElement2, rank> const& input_array2,
    PartitionedArray<InputElement3, rank> const& input_array3,
    Functor const& functor)
{
    using InputArray1 = PartitionedArray<InputElement1, rank>;
    using InputPartitions1 = PartitionsT<InputArray1>;
    using InputPartition1 = PartitionT<InputArray1>;

    using InputArray2 = PartitionedArray<InputElement2, rank>;
    using InputPartitions2 = PartitionsT<InputArray2>;
    using InputPartition2 = PartitionT<InputArray2>;

    using InputArray3 = PartitionedArray<InputElement3, rank>;
    using InputPartitions3 = PartitionsT<InputArray3>;
    using InputPartition3 = PartitionT<InputArray3>;

    using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
    using OutputPartition = PartitionT<OutputArray>;
    using OutputPartitions = PartitionsT<OutputArray>;

    lue_assert(nr_partitions(input_array1) == nr_partitions(input_array2));
    lue_assert(nr_partitions(input_array1) == nr_partitions(input_array3));
    lue_assert(shape_in_partitions(input_array1) == shape_in_partitions(input_array2));
    lue_assert(shape_in_partitions(input_array1) == shape_in_partitions(input_array3));

    detail::TernaryLocalOperationPartitionAction<
        InputPartition1, InputPartition2, InputPartition3, OutputPartition,
        Functor> action;

    Localities<rank> const& localities{input_array1.localities()};
    InputPartitions1 const& input_partitions1{input_array1.partitions()};
    InputPartitions2 const& input_partitions2{input_array2.partitions()};
    InputPartitions3 const& input_partitions3{input_array3.partitions()};
    OutputPartitions output_partitions{shape_in_partitions(input_array1)};

    for(Index p = 0; p < nr_partitions(input_array1); ++p)
    {
        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [locality_id=localities[p], action, functor](
                InputPartition1 const& input_partition1,
                InputPartition2 const& input_partition2,
                InputPartition2 const& input_partition3)
            {
                AnnotateFunction annotation{"ternary_local_operation"};

                return action(locality_id, input_partition1, input_partition2, input_partition3, functor);
            },

            input_partitions1[p],
            input_partitions2[p],
            input_partitions3[p]);
    }

    return OutputArray{shape(input_array1), localities, std::move(output_partitions)};
}


// local_operation(array, array, scalar)
template<
    typename InputElement1,
    typename InputElement2,
    typename InputElement3,
    Rank rank,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> ternary_local_operation(
    PartitionedArray<InputElement1, rank> const& input_array1,
    PartitionedArray<InputElement2, rank> const& input_array2,
    hpx::shared_future<InputElement3> const& input_scalar,
    Functor const& functor)
{
    using InputArray1 = PartitionedArray<InputElement1, rank>;
    using InputPartitions1 = PartitionsT<InputArray1>;
    using InputPartition1 = PartitionT<InputArray1>;

    using InputArray2 = PartitionedArray<InputElement2, rank>;
    using InputPartitions2 = PartitionsT<InputArray2>;
    using InputPartition2 = PartitionT<InputArray2>;

    using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
    using OutputPartition = PartitionT<OutputArray>;
    using OutputPartitions = PartitionsT<OutputArray>;

    lue_assert(nr_partitions(input_array1) == nr_partitions(input_array2));
    lue_assert(shape_in_partitions(input_array1) == shape_in_partitions(input_array2));

    detail::TernaryLocalOperationPartitionAction<
        InputPartition1, InputPartition2, InputElement3, OutputPartition,
        Functor> action;

    Localities<rank> const& localities{input_array1.localities()};
    InputPartitions1 const& input_partitions1{input_array1.partitions()};
    InputPartitions2 const& input_partitions2{input_array2.partitions()};
    OutputPartitions output_partitions{shape_in_partitions(input_array1)};

    for(Index p = 0; p < nr_partitions(input_array1); ++p)
    {
        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [locality_id=localities[p], action, functor](
                InputPartition1 const& input_partition1,
                InputPartition2 const& input_partition2,
                hpx::shared_future<InputElement3> const& input_scalar)
            {
                AnnotateFunction annotation{"ternary_local_operation"};

                return action(locality_id, input_partition1, input_partition2, input_scalar.get(), functor);
            },

            input_partitions1[p],
            input_partitions2[p],
            input_scalar);
    }

    return OutputArray{shape(input_array1), localities, std::move(output_partitions)};
}


// local_operation(array, scalar, array)
template<
    typename InputElement1,
    typename InputElement2,
    typename InputElement3,
    Rank rank,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> ternary_local_operation(
    PartitionedArray<InputElement1, rank> const& input_array1,
    hpx::shared_future<InputElement2> const& input_scalar,
    PartitionedArray<InputElement3, rank> const& input_array2,
    Functor const& functor)
{
    using InputArray1 = PartitionedArray<InputElement1, rank>;
    using InputPartitions1 = PartitionsT<InputArray1>;
    using InputPartition1 = PartitionT<InputArray1>;

    using InputArray2 = PartitionedArray<InputElement3, rank>;
    using InputPartitions2 = PartitionsT<InputArray2>;
    using InputPartition2 = PartitionT<InputArray2>;

    using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
    using OutputPartition = PartitionT<OutputArray>;
    using OutputPartitions = PartitionsT<OutputArray>;

    lue_assert(nr_partitions(input_array1) == nr_partitions(input_array2));
    lue_assert(shape_in_partitions(input_array1) == shape_in_partitions(input_array2));

    detail::TernaryLocalOperationPartitionAction<
        InputPartition1, InputElement2, InputPartition2, OutputPartition,
        Functor> action;

    Localities<rank> const& localities{input_array1.localities()};
    InputPartitions1 const& input_partitions1{input_array1.partitions()};
    InputPartitions2 const& input_partitions2{input_array2.partitions()};
    OutputPartitions output_partitions{shape_in_partitions(input_array1)};

    for(Index p = 0; p < nr_partitions(input_array1); ++p)
    {
        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [locality_id=localities[p], action, functor](
                InputPartition1 const& input_partition1,
                hpx::shared_future<InputElement2> const& input_scalar,
                InputPartition2 const& input_partition2)
            {
                AnnotateFunction annotation{"ternary_local_operation"};

                return action(locality_id, input_partition1, input_scalar.get(), input_partition2, functor);
            },

            input_partitions1[p],
            input_scalar,
            input_partitions2[p]);
    }

    return OutputArray{shape(input_array1), localities, std::move(output_partitions)};
}


// local_operation(array, scalar, scalar)
template<
    typename InputElement1,
    typename InputElement2,
    typename InputElement3,
    Rank rank,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> ternary_local_operation(
    PartitionedArray<InputElement1, rank> const& input_array,
    hpx::shared_future<InputElement2> const& input_scalar1,
    hpx::shared_future<InputElement3> const& input_scalar2,
    Functor const& functor)
{
    using InputArray = PartitionedArray<InputElement1, rank>;
    using InputPartitions = PartitionsT<InputArray>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
    using OutputPartition = PartitionT<OutputArray>;
    using OutputPartitions = PartitionsT<OutputArray>;

    detail::TernaryLocalOperationPartitionAction<
        InputPartition, InputElement2, InputElement3, OutputPartition,
        Functor> action;

    Localities<rank> const& localities{input_array.localities()};
    InputPartitions const& input_partitions{input_array.partitions()};
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p)
    {
        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [locality_id=localities[p], action, functor](
                InputPartition const& input_partition,
                hpx::shared_future<InputElement2> const& input_scalar1,
                hpx::shared_future<InputElement3> const& input_scalar2)
            {
                AnnotateFunction annotation{"ternary_local_operation"};

                return action(locality_id, input_partition, input_scalar1.get(), input_scalar2.get(), functor);
            },

            input_partitions[p],
            input_scalar1,
            input_scalar2);
    }

    return OutputArray{shape(input_array), localities, std::move(output_partitions)};
}

}  // namespace lue
