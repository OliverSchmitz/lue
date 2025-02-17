#define BOOST_TEST_MODULE lue framework algorithm focal_majority
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/range.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/framework/algorithm/value_policies/focal_majority.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(focal_majority_range)
{
    using Element = lue::LargestUnsignedIntegralElement;
    std::size_t const rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{{9, 9}};
    Shape const partition_shape{{3, 3}};

    // [ 1  2  3  4  5  6  7  8  9]
    // [10 11 12 13 14 15 16 17 18]
    // [19 20 21 22 23 24 25 26 27]
    // [28 29 30 31 32 33 34 35 36]
    // [37 38 39 40 41 42 43 44 45]
    // [46 47 48 49 50 51 52 53 54]
    // [55 56 57 58 59 60 61 62 63]
    // [64 65 66 67 68 69 70 71 72]
    // [73 74 75 76 77 78 79 80 81]
    Array array{lue::create_partitioned_array<Element>(array_shape, partition_shape)};
    lue::range(array, Element{1}).get();

    // [true true true]
    // [true true true]
    // [true true true]
    auto const kernel = lue::box_kernel<lue::BooleanElement, rank>(1, 1);
    auto focal_majority = lue::value_policies::focal_majority(array, kernel);

    Element const nd{lue::policy::no_data_value<Element>};

    Array array_we_want = lue::test::create_partitioned_array<Array>(
        array_shape,
        partition_shape,
        {
            {nd, nd, nd, nd, nd, nd, nd, nd, nd},
            {nd, nd, nd, nd, nd, nd, nd, nd, nd},
            {nd, nd, nd, nd, nd, nd, nd, nd, nd},

            {nd, nd, nd, nd, nd, nd, nd, nd, nd},
            {nd, nd, nd, nd, nd, nd, nd, nd, nd},
            {nd, nd, nd, nd, nd, nd, nd, nd, nd},

            {nd, nd, nd, nd, nd, nd, nd, nd, nd},
            {nd, nd, nd, nd, nd, nd, nd, nd, nd},
            {nd, nd, nd, nd, nd, nd, nd, nd, nd},
        });

    lue::test::check_arrays_are_equal(focal_majority, array_we_want);
}


BOOST_AUTO_TEST_CASE(focal_majority_constant)
{
    using Element = lue::LargestSignedIntegralElement;
    std::size_t const rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{{9, 9}};
    Shape const partition_shape{{3, 3}};

    Array array{lue::create_partitioned_array<Element>(array_shape, partition_shape, Element{5})};

    // [true true true]
    // [true true true]
    // [true true true]
    auto const kernel = lue::box_kernel<lue::BooleanElement, rank>(1, 1);
    auto focal_majority = lue::value_policies::focal_majority(array, kernel);

    Element const nd{lue::policy::no_data_value<Element>};

    Array array_we_want = lue::test::create_partitioned_array<Array>(
        array_shape,
        partition_shape,
        {
            {nd, nd, nd, nd, 5, 5, nd, 5, 5},
            {nd, nd, nd, 5, 5, 5, 5, 5, 5},
            {nd, nd, nd, 5, 5, nd, 5, 5, nd},

            {nd, 5, 5, nd, 5, 5, nd, 5, 5},
            {5, 5, 5, 5, 5, 5, 5, 5, 5},
            {5, 5, nd, 5, 5, nd, 5, 5, nd},

            {nd, 5, 5, nd, 5, 5, nd, nd, nd},
            {5, 5, 5, 5, 5, 5, nd, nd, nd},
            {5, 5, nd, 5, 5, nd, nd, nd, nd},
        });

    lue::test::check_arrays_are_equal(focal_majority, array_we_want);
}
