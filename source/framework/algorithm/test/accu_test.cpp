#define BOOST_TEST_MODULE lue framework algorithm accu
#include "lue/framework/algorithm/definition/accu.hpp"
#include "lue/framework/algorithm/definition/accu3.hpp"
#include "flow_accumulation.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace {

    // Grab symbols from flow_accumulation header
    using namespace lue::test;

    std::size_t const rank = 2;

    using MaterialElement = float;
    using MaterialArray = lue::PartitionedArray<MaterialElement, rank>;

    template<
        typename Policies,
        typename FlowDirectionElement,
        typename MaterialElement,
        lue::Rank rank>
    void test_accu(
        Policies const& policies,
        lue::PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        lue::PartitionedArray<MaterialElement, rank> const& material,
        lue::PartitionedArray<MaterialElement, rank> const& accu_we_want)
    {
        using MaterialArray = lue::PartitionedArray<MaterialElement, rank>;

        MaterialArray accu_we_got = lue::accu(policies, flow_direction, material);

        lue::test::check_arrays_are_equal(accu_we_got, accu_we_want);
    }


    template<
        typename Policies,
        typename FlowDirectionElement,
        typename MaterialElement,
        lue::Rank rank>
    void test_accu3(
        Policies const& policies,
        lue::PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        lue::PartitionedArray<MaterialElement, rank> const& material,
        lue::PartitionedArray<MaterialElement, rank> const& accu_we_want)
    {
        using MaterialArray = lue::PartitionedArray<MaterialElement, rank>;

        MaterialArray accu_we_got = lue::accu3(policies, flow_direction, material);

        lue::test::check_arrays_are_equal(accu_we_got, accu_we_want);
    }


    template<
        typename FlowDirectionElement,
        typename MaterialElement,
        lue::Rank rank>
    void test_accu(
        lue::PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        lue::PartitionedArray<MaterialElement, rank> const& material,
        lue::PartitionedArray<MaterialElement, rank> const& accu_we_want)
    {
        {
            using Policies = lue::policy::accu::DefaultPolicies<FlowDirectionElement, MaterialElement>;

            test_accu(Policies{}, flow_direction, material, accu_we_want);
        }

        {
            using Policies = lue::policy::accu3::DefaultPolicies<FlowDirectionElement, MaterialElement>;

            test_accu3(Policies{}, flow_direction, material, accu_we_want);
        }
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(parallel_east)
{
    test_accu(
        lue::test::create_partitioned_array<FlowDirectionArray>(array_shape, partition_shape,
            {
                { e, e, e,
                  e, e, e,
                  e, e, e, },
                { e, e, e,
                  e, e, e,
                  e, e, e, },
                { e, e, e,
                  e, e, e,
                  e, e, e, },
                { e, e, e,
                  e, e, e,
                  e, e, e, },
                { e, e, e,
                  e, e, e,
                  e, e, e, },
                { e, e, e,
                  e, e, e,
                  e, e, e, },
                { e, e, e,
                  e, e, e,
                  e, e, e, },
                { e, e, e,
                  e, e, e,
                  e, e, e, },
                { e, e, e,
                  e, e, e,
                  e, e, e, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 1, 2, 3,
                  1, 2, 3,
                  1, 2, 3, },
                { 4, 5, 6,
                  4, 5, 6,
                  4, 5, 6, },
                { 7, 8, 9,
                  7, 8, 9,
                  7, 8, 9, },
                { 1, 2, 3,
                  1, 2, 3,
                  1, 2, 3, },
                { 4, 5, 6,
                  4, 5, 6,
                  4, 5, 6, },
                { 7, 8, 9,
                  7, 8, 9,
                  7, 8, 9, },
                { 1, 2, 3,
                  1, 2, 3,
                  1, 2, 3, },
                { 4, 5, 6,
                  4, 5, 6,
                  4, 5, 6, },
                { 7, 8, 9,
                  7, 8, 9,
                  7, 8, 9, },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_south_east)
{
    test_accu(
        lue::test::create_partitioned_array<FlowDirectionArray>(array_shape, partition_shape,
            {
                { se, se, se,
                  se, se, se,
                  se, se, se, },
                { se, se, se,
                  se, se, se,
                  se, se, se, },
                { se, se, se,
                  se, se, se,
                  se, se, se, },
                { se, se, se,
                  se, se, se,
                  se, se, se, },
                { se, se, se,
                  se, se, se,
                  se, se, se, },
                { se, se, se,
                  se, se, se,
                  se, se, se, },
                { se, se, se,
                  se, se, se,
                  se, se, se, },
                { se, se, se,
                  se, se, se,
                  se, se, se, },
                { se, se, se,
                  se, se, se,
                  se, se, se, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 1, 1, 1,
                  1, 2, 2,
                  1, 2, 3, },
                { 1, 1, 1,
                  2, 2, 2,
                  3, 3, 3, },
                { 1, 1, 1,
                  2, 2, 2,
                  3, 3, 3, },
                { 1, 2, 3,
                  1, 2, 3,
                  1, 2, 3, },
                { 4, 4, 4,
                  4, 5, 5,
                  4, 5, 6, },
                { 4, 4, 4,
                  5, 5, 5,
                  6, 6, 6, },
                { 1, 2, 3,
                  1, 2, 3,
                  1, 2, 3, },
                { 4, 5, 6,
                  4, 5, 6,
                  4, 5, 6, },
                { 7, 7, 7,
                  7, 8, 8,
                  7, 8, 9, },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_south)
{
    test_accu(
        lue::test::create_partitioned_array<FlowDirectionArray>(array_shape, partition_shape,
            {
                { s, s, s,
                  s, s, s,
                  s, s, s, },
                { s, s, s,
                  s, s, s,
                  s, s, s, },
                { s, s, s,
                  s, s, s,
                  s, s, s, },
                { s, s, s,
                  s, s, s,
                  s, s, s, },
                { s, s, s,
                  s, s, s,
                  s, s, s, },
                { s, s, s,
                  s, s, s,
                  s, s, s, },
                { s, s, s,
                  s, s, s,
                  s, s, s, },
                { s, s, s,
                  s, s, s,
                  s, s, s, },
                { s, s, s,
                  s, s, s,
                  s, s, s, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 1, 1, 1,
                  2, 2, 2,
                  3, 3, 3, },
                { 1, 1, 1,
                  2, 2, 2,
                  3, 3, 3, },
                { 1, 1, 1,
                  2, 2, 2,
                  3, 3, 3, },
                { 4, 4, 4,
                  5, 5, 5,
                  6, 6, 6, },
                { 4, 4, 4,
                  5, 5, 5,
                  6, 6, 6, },
                { 4, 4, 4,
                  5, 5, 5,
                  6, 6, 6, },
                { 7, 7, 7,
                  8, 8, 8,
                  9, 9, 9, },
                { 7, 7, 7,
                  8, 8, 8,
                  9, 9, 9, },
                { 7, 7, 7,
                  8, 8, 8,
                  9, 9, 9, },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_south_west)
{
    test_accu(
        lue::test::create_partitioned_array<FlowDirectionArray>(array_shape, partition_shape,
            {
                { sw, sw, sw,
                  sw, sw, sw,
                  sw, sw, sw, },
                { sw, sw, sw,
                  sw, sw, sw,
                  sw, sw, sw, },
                { sw, sw, sw,
                  sw, sw, sw,
                  sw, sw, sw, },
                { sw, sw, sw,
                  sw, sw, sw,
                  sw, sw, sw, },
                { sw, sw, sw,
                  sw, sw, sw,
                  sw, sw, sw, },
                { sw, sw, sw,
                  sw, sw, sw,
                  sw, sw, sw, },
                { sw, sw, sw,
                  sw, sw, sw,
                  sw, sw, sw, },
                { sw, sw, sw,
                  sw, sw, sw,
                  sw, sw, sw, },
                { sw, sw, sw,
                  sw, sw, sw,
                  sw, sw, sw, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 1, 1, 1,
                  2, 2, 2,
                  3, 3, 3, },
                { 1, 1, 1,
                  2, 2, 2,
                  3, 3, 3, },
                { 1, 1, 1,
                  2, 2, 1,
                  3, 2, 1, },
                { 4, 4, 4,
                  5, 5, 5,
                  6, 6, 6, },
                { 4, 4, 4,
                  5, 5, 4,
                  6, 5, 4, },
                { 3, 2, 1,
                  3, 2, 1,
                  3, 2, 1, },
                { 7, 7, 7,
                  8, 8, 7,
                  9, 8, 7, },
                { 6, 5, 4,
                  6, 5, 4,
                  6, 5, 4, },
                { 3, 2, 1,
                  3, 2, 1,
                  3, 2, 1, },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_west)
{
    test_accu(
        lue::test::create_partitioned_array<FlowDirectionArray>(array_shape, partition_shape,
            {
                { w, w, w,
                  w, w, w,
                  w, w, w, },
                { w, w, w,
                  w, w, w,
                  w, w, w, },
                { w, w, w,
                  w, w, w,
                  w, w, w, },
                { w, w, w,
                  w, w, w,
                  w, w, w, },
                { w, w, w,
                  w, w, w,
                  w, w, w, },
                { w, w, w,
                  w, w, w,
                  w, w, w, },
                { w, w, w,
                  w, w, w,
                  w, w, w, },
                { w, w, w,
                  w, w, w,
                  w, w, w, },
                { w, w, w,
                  w, w, w,
                  w, w, w, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 9, 8, 7,
                  9, 8, 7,
                  9, 8, 7, },
                { 6, 5, 4,
                  6, 5, 4,
                  6, 5, 4, },
                { 3, 2, 1,
                  3, 2, 1,
                  3, 2, 1, },
                { 9, 8, 7,
                  9, 8, 7,
                  9, 8, 7, },
                { 6, 5, 4,
                  6, 5, 4,
                  6, 5, 4, },
                { 3, 2, 1,
                  3, 2, 1,
                  3, 2, 1, },
                { 9, 8, 7,
                  9, 8, 7,
                  9, 8, 7, },
                { 6, 5, 4,
                  6, 5, 4,
                  6, 5, 4, },
                { 3, 2, 1,
                  3, 2, 1,
                  3, 2, 1, },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_north_west)
{
    test_accu(
        lue::test::create_partitioned_array<FlowDirectionArray>(array_shape, partition_shape,
            {
                { nw, nw, nw,
                  nw, nw, nw,
                  nw, nw, nw, },
                { nw, nw, nw,
                  nw, nw, nw,
                  nw, nw, nw, },
                { nw, nw, nw,
                  nw, nw, nw,
                  nw, nw, nw, },
                { nw, nw, nw,
                  nw, nw, nw,
                  nw, nw, nw, },
                { nw, nw, nw,
                  nw, nw, nw,
                  nw, nw, nw, },
                { nw, nw, nw,
                  nw, nw, nw,
                  nw, nw, nw, },
                { nw, nw, nw,
                  nw, nw, nw,
                  nw, nw, nw, },
                { nw, nw, nw,
                  nw, nw, nw,
                  nw, nw, nw, },
                { nw, nw, nw,
                  nw, nw, nw,
                  nw, nw, nw, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 9, 8, 7,
                  8, 8, 7,
                  7, 7, 7, },
                { 6, 5, 4,
                  6, 5, 4,
                  6, 5, 4, },
                { 3, 2, 1,
                  3, 2, 1,
                  3, 2, 1, },
                { 6, 6, 6,
                  5, 5, 5,
                  4, 4, 4, },
                { 6, 5, 4,
                  5, 5, 4,
                  4, 4, 4, },
                { 3, 2, 1,
                  3, 2, 1,
                  3, 2, 1, },
                { 3, 3, 3,
                  2, 2, 2,
                  1, 1, 1, },
                { 3, 3, 3,
                  2, 2, 2,
                  1, 1, 1, },
                { 3, 2, 1,
                  2, 2, 1,
                  1, 1, 1, },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_north)
{
    test_accu(
        lue::test::create_partitioned_array<FlowDirectionArray>(array_shape, partition_shape,
            {
                { n, n, n,
                  n, n, n,
                  n, n, n, },
                { n, n, n,
                  n, n, n,
                  n, n, n, },
                { n, n, n,
                  n, n, n,
                  n, n, n, },
                { n, n, n,
                  n, n, n,
                  n, n, n, },
                { n, n, n,
                  n, n, n,
                  n, n, n, },
                { n, n, n,
                  n, n, n,
                  n, n, n, },
                { n, n, n,
                  n, n, n,
                  n, n, n, },
                { n, n, n,
                  n, n, n,
                  n, n, n, },
                { n, n, n,
                  n, n, n,
                  n, n, n, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 9, 9, 9,
                  8, 8, 8,
                  7, 7, 7, },
                { 9, 9, 9,
                  8, 8, 8,
                  7, 7, 7, },
                { 9, 9, 9,
                  8, 8, 8,
                  7, 7, 7, },
                { 6, 6, 6,
                  5, 5, 5,
                  4, 4, 4, },
                { 6, 6, 6,
                  5, 5, 5,
                  4, 4, 4, },
                { 6, 6, 6,
                  5, 5, 5,
                  4, 4, 4, },
                { 3, 3, 3,
                  2, 2, 2,
                  1, 1, 1, },
                { 3, 3, 3,
                  2, 2, 2,
                  1, 1, 1, },
                { 3, 3, 3,
                  2, 2, 2,
                  1, 1, 1, },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_north_east)
{
    test_accu(
        lue::test::create_partitioned_array<FlowDirectionArray>(array_shape, partition_shape,
            {
                { ne, ne, ne,
                  ne, ne, ne,
                  ne, ne, ne, },
                { ne, ne, ne,
                  ne, ne, ne,
                  ne, ne, ne, },
                { ne, ne, ne,
                  ne, ne, ne,
                  ne, ne, ne, },
                { ne, ne, ne,
                  ne, ne, ne,
                  ne, ne, ne, },
                { ne, ne, ne,
                  ne, ne, ne,
                  ne, ne, ne, },
                { ne, ne, ne,
                  ne, ne, ne,
                  ne, ne, ne, },
                { ne, ne, ne,
                  ne, ne, ne,
                  ne, ne, ne, },
                { ne, ne, ne,
                  ne, ne, ne,
                  ne, ne, ne, },
                { ne, ne, ne,
                  ne, ne, ne,
                  ne, ne, ne, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 1, 2, 3,
                  1, 2, 3,
                  1, 2, 3, },
                { 4, 5, 6,
                  4, 5, 6,
                  4, 5, 6, },
                { 7, 8, 9,
                  7, 8, 8,
                  7, 7, 7, },
                { 1, 2, 3,
                  1, 2, 3,
                  1, 2, 3, },
                { 4, 5, 6,
                  4, 5, 5,
                  4, 4, 4, },
                { 6, 6, 6,
                  5, 5, 5,
                  4, 4, 4, },
                { 1, 2, 3,
                  1, 2, 2,
                  1, 1, 1, },
                { 3, 3, 3,
                  2, 2, 2,
                  1, 1, 1, },
                { 3, 3, 3,
                  2, 2, 2,
                  1, 1, 1, },
            }));
}


BOOST_AUTO_TEST_CASE(converge)
{
    test_accu(
        lue::test::create_partitioned_array<FlowDirectionArray>(array_shape, partition_shape,
            {
                { se, se, se,
                  se, se, se,
                  se, se, se, },
                {  s,  s,  s,
                   s,  s,  s,
                   s,  s,  s, },
                { sw, sw, sw,
                  sw, sw, sw,
                  sw, sw, sw, },
                {  e,  e,  e,
                   e,  e,  e,
                   e,  e,  e, },
                { se,  s, sw,
                   e,  p, w,
                  ne,  n, nw, },
                {  w,  w,  w,
                   w,  w,  w,
                   w,  w,  w, },
                { ne, ne, ne,
                  ne, ne, ne,
                  ne, ne, ne, },
                {  n,  n,  n,
                   n,  n,  n,
                   n,  n,  n, },
                { nw, nw, nw,
                  nw, nw, nw,
                  nw, nw, nw, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                {  1,  1,  1,
                   1,  2,  2,
                   1,  2,  3, },
                {  1,  1,  1,
                   3,  2,  3,
                   6,  3,  6, },
                {  1,  1,  1,
                   2,  2,  1,
                   3,  2,  1, },
                {  1,  3,  6,
                   1,  2,  3,
                   1,  3,  6, },
                { 16,  4, 16,
                   4, 81,  4,
                  16,  4, 16, },
                {  6,  3,  1,
                   3,  2,  1,
                   6,  3,  1, },
                {  1,  2,  3,
                   1,  2,  2,
                   1,  1,  1, },
                {  6,  3,  6,
                   3,  2,  3,
                   1,  1,  1, },
                {  3,  2,  1,
                   2,  2,  1,
                   1,  1,  1, },
            }));
}


BOOST_AUTO_TEST_CASE(diverge)
{
    test_accu(
        lue::test::create_partitioned_array<FlowDirectionArray>(array_shape, partition_shape,
            {
                { nw, nw, nw,
                  nw, nw, nw,
                  nw, nw, nw, },
                {  n,  n,  n,
                   n,  n,  n,
                   n,  n,  n, },
                { ne, ne, ne,
                  ne, ne, ne,
                  ne, ne, ne, },
                {  w,  w,  w,
                   w,  w,  w,
                   w,  w,  w, },
                { nw,  n, ne,
                   w,  s,  e,
                  sw,  s, se, },
                {  e,  e,  e,
                   e,  e,  e,
                   e,  e,  e, },
                { sw, sw, sw,
                  sw, sw, sw,
                  sw, sw, sw, },
                {  s,  s,  s,
                   s,  s,  s,
                   s,  s,  s, },
                { se, se, se,
                  se, se, se,
                  se, se, se, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                {  4,  2,  1,
                   2,  3,  1,
                   1,  1,  2, },
                {  3,  4,  3,
                   2,  3,  2,
                   1,  2,  1, },
                {  1,  2,  4,
                   1,  3,  2,
                   2,  1,  1, },
                {  3,  2,  1,
                   4,  3,  2,
                   3,  2,  1, },
                {  1,  1,  1,
                   1,  1,  1,
                   1,  2,  1, },
                {  1,  2,  3,
                   2,  3,  4,
                   1,  2,  3, },
                {  1,  1,  2,
                   2,  3,  1,
                   4,  2,  1, },
                {  1,  3,  1,
                   2,  4,  2,
                   3,  5,  3, },
                {  2,  1,  1,
                   1,  3,  2,
                   1,  2,  4, },
            }));
}


BOOST_AUTO_TEST_CASE(spiral_in_case)
{
    test_accu(
        lue::test::spiral_in(),
        lue::test::ones<MaterialElement>(),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                {  1,  2,  3,
                  32, 33, 34,
                  31, 56, 57, },
                {  4,  5,  6,
                  35, 36, 37,
                  58, 59, 60, },
                {  7,  8,  9,
                  38, 39, 10,
                  61, 40, 11, },
                { 30, 55, 72,
                  29, 54, 71,
                  28, 53, 70, },
                { 73, 74, 75,
                  80, 81, 76,
                  79, 78, 77, },
                { 62, 41, 12,
                  63, 42, 13,
                  64, 43, 14, },
                { 27, 52, 69,
                  26, 51, 50,
                  25, 24, 23, },
                { 68, 67, 66,
                  49, 48, 47,
                  22, 21, 20, },
                { 65, 44, 15,
                  46, 45, 16,
                  19, 18, 17, },
            }));
}


BOOST_AUTO_TEST_CASE(spiral_out)
{
    test_accu(
        lue::test::create_partitioned_array<FlowDirectionArray>(array_shape, partition_shape,
            {
                {  w,  w,  w,
                   s,  w,  w,
                   s,  s,  w, },
                {  w,  w,  w,
                   w,  w,  w,
                   w,  w,  w, },
                {  w,  w,  w,
                   w,  w,  n,
                   w,  n,  n, },
                {  s,  s,  s,
                   s,  s,  s,
                   s,  s,  s, },
                {  w,  w,  w,
                   s,  w,  n,
                   e,  e,  n, },
                {  n,  n,  n,
                   n,  n,  n,
                   n,  n,  n, },
                {  s,  s,  e,
                   s,  e,  e,
                   e,  e,  e, },
                {  e,  e,  e,
                   e,  e,  e,
                   e,  e,  e, },
                {  n,  n,  n,
                   e,  n,  n,
                   e,  e,  n, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
                { 1, 1, 1,
                  1, 1, 1,
                  1, 1, 1, },
            }),
        lue::test::create_partitioned_array<MaterialArray>(array_shape, partition_shape,
            {
                { 81, 80, 79,
                  50, 49, 48,
                  51, 26, 25, },
                { 78, 77, 76,
                  47, 46, 45,
                  24, 23, 22, },
                { 75, 74, 73,
                  44, 43, 72,
                  21, 42, 71, },
                { 52, 27, 10,
                  53, 28, 11,
                  54, 29, 12, },
                { 9 , 8 , 7 ,
                  2 , 1 , 6 ,
                  3 , 4 , 5 , },
                { 20, 41, 70,
                  19, 40, 69,
                  18, 39, 68, },
                { 55, 30, 13,
                  56, 31, 32,
                  57, 58, 59, },
                { 14, 15, 16,
                  33, 34, 35,
                  60, 61, 62, },
                { 17, 38, 67,
                  36, 37, 66,
                  63, 64, 65, },
            }));
}


BOOST_AUTO_TEST_CASE(all_no_data_flow_direction)
{
    auto const flow_direction = lue::test::no_data<FlowDirectionElement>();
    auto const material = lue::test::ones<MaterialElement>();
    auto const result_we_want = lue::test::filled(lue::policy::no_data_value<MaterialElement>);

    {
        using Policies = lue::policy::accu::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;
        Policies policies{};

        test_accu(policies, flow_direction, material, result_we_want);
    }

    {
        using Policies = lue::policy::accu3::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

        test_accu3(Policies{}, flow_direction, material, result_we_want);
    }
}


BOOST_AUTO_TEST_CASE(all_no_data_material)
{
    auto const flow_direction = lue::test::filled(e);
    auto const material = lue::test::no_data<MaterialElement>();
    auto const result_we_want = lue::test::filled(lue::policy::no_data_value<MaterialElement>);

    {
        using Policies = lue::policy::accu::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;
        Policies policies{};

        test_accu(policies, flow_direction, material, result_we_want);
    }

    {
        using Policies = lue::policy::accu3::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

        test_accu3(Policies{}, flow_direction, material, result_we_want);
    }
}


BOOST_AUTO_TEST_CASE(merging_streams_case_01)
{
    // No-data material in ridge cell. From there on, it must propage
    // down to all downstream cells, in all downstream partitions.

    auto flow_direction = lue::test::merging_streams();

    auto const x{lue::policy::no_data_value<MaterialElement>};

    auto material = create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape, partition_shape,
        {
            { // 0, 0
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            { // 0, 1
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            { // 0, 2
                1, x, 1,
                1, 1, 1,
                1, 1, 1,
            },
            { // 1, 0
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            { // 1, 1
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            { // 1, 2
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            { // 2, 0
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            { // 2, 1
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            { // 2, 2
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
        });

    auto result_we_want = create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape, partition_shape,
        {
            { // 0, 0
                x, x, x,
                x, x, x,
                x, x, 1,
            },
            { // 0, 1
                x, 1, 1,
                x, 1, 1,
                x, 2, x,
            },
            { // 0, 2
                x, x, 1,
                3, x, 1,
                x, x, 1,
            },
            { // 1, 0
                x, x, 1,
                x, x, 1,
                x, x, 1,
            },
            { // 1, 1
                4, x, x,
                x, x, 2,
                2, x, 3,
            },
            { // 1, 2
                x, x, x,
                1, x, x,
                1, x, x,
            },
            { // 2, 0
                x, x, x,
                x, x, x,
                x, x, x,
            },
            { // 2, 1
                x, x, x,
                x, x, 2,
                4, 3, 2,
            },
            { // 2, 2
                1, x, x,
                1, x, x,
                1, x, x,
            },
        });

    {
        using Policies = lue::policy::accu::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;
        Policies policies{};

        test_accu(policies, flow_direction, material, result_we_want);
    }

    {
        using Policies = lue::policy::accu3::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

        test_accu3(Policies{}, flow_direction, material, result_we_want);
    }
}


// TODO Out of domain values: negative material is out of domain!
