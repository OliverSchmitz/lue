#define BOOST_TEST_MODULE lue framework algorithm convolve
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/convolve.hpp"
#include "lue/framework/algorithm/fill.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


// BOOST_AUTO_TEST_CASE(window_total_1d)
// {
//     using Element = std::int32_t;
//     std::size_t const rank = 1;
// 
//     using Array = lue::PartitionedArray<Element, rank>;
//     using Shape = lue::ShapeT<Array>;
// 
//     Shape const array_shape{{8}};
//     Shape const partition_shape{{3}};
// 
//     // |           |           |       |
//     // +---+---+---+---+---+---+---+---+
//     // | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 |
//     // +---+---+---+---+---+---+---+---+
//     // |           |           |       |
//     Array array{array_shape, partition_shape};
//     BOOST_REQUIRE_EQUAL(lue::nr_partitions(array), 3);
//     lue::fill(array, hpx::make_ready_future<Element>(1).share()).wait();
// 
//     // +---+---+---+
//     // | 1 | 1 | 1 |
//     // +---+---+---+
//     auto const kernel = lue::box_kernel<bool, rank>(1, true);
// 
//     // |           |           |       |
//     // +---+---+---+---+---+---+---+---+
//     // | 2 | 3 | 3 | 3 | 3 | 3 | 3 | 2 |
//     // +---+---+---+---+---+---+---+---+
//     // |           |           |       |
//     auto convolve = lue::convolve(array, kernel);
// 
//     static_assert(std::is_same_v<lue::ElementT<decltype(convolve)>, double>);
// 
//     BOOST_REQUIRE_EQUAL(lue::nr_partitions(convolve), 3);
//     BOOST_REQUIRE_EQUAL(convolve.shape(), array_shape);
// 
//     // First partition
//     {
//         // TODO
//         // auto const& partition = convolve.partitions()[0];
//         // auto const data = partition.data(lue::CopyMode::share).get();
//         // BOOST_REQUIRE_EQUAL(shape(data), partition_shape);
//         // BOOST_CHECK_EQUAL(data[0], 2.0);
//         // BOOST_CHECK_EQUAL(data[1], 3.0);
//         // BOOST_CHECK_EQUAL(data[2], 3.0);
//     }
// 
//     // Second partition
//     {
//         auto const& partition = convolve.partitions()[1];
//         auto const data = partition.data(lue::CopyMode::share).get();
//         BOOST_REQUIRE_EQUAL(shape(data), partition_shape);
//         BOOST_CHECK_EQUAL(data[0], 3.0);
//         BOOST_CHECK_EQUAL(data[1], 3.0);
//         BOOST_CHECK_EQUAL(data[2], 3.0);
//     }
// 
//     // Third partition
//     {
//         // TODO
//         // auto const& partition = convolve.partitions()[2];
//         // auto const data = partition.data(lue::CopyMode::share).get();
//         // BOOST_REQUIRE_EQUAL(shape(data), Shape{{2}});
//         // BOOST_CHECK_EQUAL(data[0], 3.0);
//         // BOOST_CHECK_EQUAL(data[1], 2.0);
//     }
// }


// TODO Test with larger kernel radius
// TODO Test with different float weights

// TODO Test empty input raster
//
// TODO First row in each partitions is fragile. Sometimes it works,
//      sometimes it doesn't...


BOOST_AUTO_TEST_CASE(window_total_2d)
{
    using Element = std::int32_t;
    std::size_t const rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using OutputArray = lue::PartitionedArray<double, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{{9, 9}};
    Shape const partition_shape{{3, 3}};

    // [1. 1. 1. 1. 1. 1. 1. 1. 1.]
    // [1. 1. 1. 1. 1. 1. 1. 1. 1.]
    // [1. 1. 1. 1. 1. 1. 1. 1. 1.]
    // [1. 1. 1. 1. 1. 1. 1. 1. 1.]
    // [1. 1. 1. 1. 1. 1. 1. 1. 1.]
    // [1. 1. 1. 1. 1. 1. 1. 1. 1.]
    // [1. 1. 1. 1. 1. 1. 1. 1. 1.]
    // [1. 1. 1. 1. 1. 1. 1. 1. 1.]
    // [1. 1. 1. 1. 1. 1. 1. 1. 1.]
    Array array{array_shape, partition_shape};
    lue::fill(array, hpx::make_ready_future<Element>(1).share()).wait();

    // [true true true]
    // [true true true]
    // [true true true]
    auto const kernel = lue::box_kernel<bool, rank>(1, true);

    // [4. 6. 6. 6. 6. 6. 6. 6. 4.]
    // [6. 9. 9. 9. 9. 9. 9. 9. 6.]
    // [6. 9. 9. 9. 9. 9. 9. 9. 6.]
    // [6. 9. 9. 9. 9. 9. 9. 9. 6.]
    // [6. 9. 9. 9. 9. 9. 9. 9. 6.]
    // [6. 9. 9. 9. 9. 9. 9. 9. 6.]
    // [6. 9. 9. 9. 9. 9. 9. 9. 6.]
    // [6. 9. 9. 9. 9. 9. 9. 9. 6.]
    // [4. 6. 6. 6. 6. 6. 6. 6. 4.]
    auto convolve = lue::convolve(array, kernel);

    OutputArray array_we_want =
            lue::test::create_partitioned_array<OutputArray>(
        array_shape, partition_shape, {
            { 4., 6., 6., 6., 9., 9., 6., 9., 9. },
            { 6., 6., 6., 9., 9., 9., 9., 9., 9. },
            { 6., 6., 4., 9., 9., 6., 9., 9., 6. },
            { 6., 9., 9., 6., 9., 9., 6., 9., 9. },
            { 9., 9., 9., 9., 9., 9., 9., 9., 9. },
            { 9., 9., 6., 9., 9., 6., 9., 9., 6. },
            { 6., 9., 9., 6., 9., 9., 4., 6., 6. },
            { 9., 9., 9., 9., 9., 9., 6., 6., 6. },
            { 9., 9., 6., 9., 9., 6., 6., 6., 4. },
        });

    lue::test::check_arrays_are_equal(convolve, array_we_want);
}


BOOST_AUTO_TEST_CASE(window_total_2d_single_partition)
{
    using Element = std::int32_t;
    std::size_t const rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using OutputArray = lue::PartitionedArray<double, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{{3, 3}};
    Shape const partition_shape{{3, 3}};

    // [1. 1. 1.]
    // [1. 1. 1.]
    // [1. 1. 1.]
    Array array{array_shape, partition_shape};
    lue::fill(array, hpx::make_ready_future<Element>(1).share()).wait();

    // [true true true]
    // [true true true]
    // [true true true]
    auto const kernel = lue::box_kernel<bool, rank>(1, true);

    // [4. 6. 4.]
    // [6. 9. 6.]
    // [4. 6. 4.]
    auto convolve = lue::convolve(array, kernel);

    OutputArray array_we_want =
            lue::test::create_partitioned_array<OutputArray>(
        array_shape, partition_shape, {
            { 4., 6., 4., 6., 9., 6., 4., 6., 4. },
        });

    lue::test::check_arrays_are_equal(convolve, array_we_want);
}


BOOST_AUTO_TEST_CASE(window_total_2d_single_row_of_partitions)
{
    using Element = std::int32_t;
    std::size_t const rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using OutputArray = lue::PartitionedArray<double, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{{3, 9}};
    Shape const partition_shape{{3, 3}};

    // [1. 1. 1. 1. 1. 1. 1. 1. 1.]
    // [1. 1. 1. 1. 1. 1. 1. 1. 1.]
    // [1. 1. 1. 1. 1. 1. 1. 1. 1.]
    Array array{array_shape, partition_shape};
    lue::fill(array, hpx::make_ready_future<Element>(1).share()).wait();

    // [true true true]
    // [true true true]
    // [true true true]
    auto const kernel = lue::box_kernel<bool, rank>(1, true);

    // [4. 6. 6. 6. 6. 6. 6. 6. 4.]
    // [6. 9. 9. 9. 9. 9. 9. 9. 6.]
    // [4. 6. 6. 6. 6. 6. 6. 6. 4.]
    auto convolve = lue::convolve(array, kernel);

    OutputArray array_we_want =
            lue::test::create_partitioned_array<OutputArray>(
        array_shape, partition_shape, {
            { 4., 6., 6., 6., 9., 9., 4., 6., 6. },
            { 6., 6., 6., 9., 9., 9., 6., 6., 6. },
            { 6., 6., 4., 9., 9., 6., 6., 6., 4. },
        });

    lue::test::check_arrays_are_equal(convolve, array_we_want);
}


BOOST_AUTO_TEST_CASE(window_total_2d_single_col_of_partitions)
{
    using Element = std::int32_t;
    std::size_t const rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using OutputArray = lue::PartitionedArray<double, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{{9, 3}};
    Shape const partition_shape{{3, 3}};

    // [1. 1. 1.]
    // [1. 1. 1.]
    // [1. 1. 1.]
    // [1. 1. 1.]
    // [1. 1. 1.]
    // [1. 1. 1.]
    // [1. 1. 1.]
    // [1. 1. 1.]
    // [1. 1. 1.]
    Array array{array_shape, partition_shape};
    lue::fill(array, hpx::make_ready_future<Element>(1).share()).wait();

    // [true true true]
    // [true true true]
    // [true true true]
    auto const kernel = lue::box_kernel<bool, rank>(1, true);

    // [4. 6. 4.]
    // [6. 9. 6.]
    // [6. 9. 6.]
    // [6. 9. 6.]
    // [6. 9. 6.]
    // [6. 9. 6.]
    // [6. 9. 6.]
    // [6. 9. 6.]
    // [4. 6. 4.]
    auto convolve = lue::convolve(array, kernel);

    OutputArray array_we_want =
            lue::test::create_partitioned_array<OutputArray>(
        array_shape, partition_shape, {
            { 4., 6., 4., 6., 9., 6., 6., 9., 6. },
            { 6., 9., 6., 6., 9., 6., 6., 9., 6. },
            { 6., 9., 6., 6., 9., 6., 4., 6., 4. },
        });

    lue::test::check_arrays_are_equal(convolve, array_we_want);
}
