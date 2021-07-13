#define BOOST_TEST_MODULE lue framework algorithm inflow_count
#include "lue/framework/algorithm/inflow_count.hpp"
#include "lue/framework/algorithm/definition/inflow_count3.hpp"
#include "flow_accumulation.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace {

    using FlowDirectionElement = std::uint32_t;
    using CountElement = std::int32_t;
    std::size_t const rank = 2;

    using FlowDirection = lue::PartitionedArray<FlowDirectionElement, rank>;
    using InflowCount = lue::PartitionedArray<CountElement, rank>;
    using Shape = lue::ShapeT<FlowDirection>;

    Shape const array_shape{{3, 3}};
    Shape const partition_shape{{3, 3}};

    auto const n{lue::north<FlowDirectionElement>};
    auto const ne{lue::north_east<FlowDirectionElement>};
    auto const e{lue::east<FlowDirectionElement>};
    auto const se{lue::south_east<FlowDirectionElement>};
    auto const s{lue::south<FlowDirectionElement>};
    auto const sw{lue::south_west<FlowDirectionElement>};
    auto const w{lue::west<FlowDirectionElement>};
    auto const nw{lue::north_west<FlowDirectionElement>};
    auto const p{lue::sink<FlowDirectionElement>};
    auto const nd{lue::no_data<FlowDirectionElement>};


    template<
        typename Policies,
        typename FlowDirectionArray,
        typename InflowCountArray>
    void test_inflow_count(
        Policies const& policies,
        FlowDirectionArray const& flow_direction,
        InflowCountArray const& inflow_count_we_want)
    {
        using CountElement = lue::ElementT<InflowCountArray>;

        InflowCount inflow_count_we_got = lue::inflow_count<CountElement>(policies, flow_direction);

        lue::test::check_arrays_are_equal(inflow_count_we_got, inflow_count_we_want);
    }


    template<
        typename Policies,
        typename FlowDirectionArray,
        typename InflowCountArray>
    void test_inflow_count3(
        Policies const& policies,
        FlowDirectionArray const& flow_direction,
        InflowCountArray const& inflow_count_we_want)
    {
        using CountElement = lue::ElementT<InflowCountArray>;

        InflowCount inflow_count_we_got = lue::inflow_count3<CountElement>(policies, flow_direction);

        lue::test::check_arrays_are_equal(inflow_count_we_got, inflow_count_we_want);
    }


    template<
        typename FlowDirectionArray,
        typename InflowCountArray>
    void test_inflow_count(
        FlowDirectionArray const& flow_direction,
        InflowCountArray const& inflow_count_we_want)
    {
        using CountElement = lue::ElementT<InflowCountArray>;
        using FlowDirectionElement = lue::ElementT<FlowDirectionArray>;

        {
            using Policies = lue::policy::inflow_count::DefaultPolicies<CountElement, FlowDirectionElement>;

            test_inflow_count(Policies{}, flow_direction, inflow_count_we_want);
        }

        {
            using Policies = lue::policy::inflow_count3::DefaultPolicies<CountElement, FlowDirectionElement>;

            test_inflow_count3(Policies{}, flow_direction, inflow_count_we_want);
        }
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(pit)
{
    // +---+---+---+   +---+---+---+
    // | 🡦 | 🡣 | 🡧 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡢 | ■ | 🡠 | → | 0 | 8 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡥 | 🡡 | 🡤 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {{
                se,  s, sw,
                 e,  p,  w,
                ne,  n, nw,
            }}),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {{
                0, 0, 0,
                0, 8, 0,
                0, 0, 0,
            }}));
}


BOOST_AUTO_TEST_CASE(peak)
{
    // +---+---+---+   +---+---+---+
    // | 🡤 | 🡡 | 🡥 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡠 | 🡣 | 🡢 | → | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡧 | 🡣 | 🡦 |   | 0 | 1 | 0 |
    // +---+---+---+   +---+---+---+
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {{
                nw,  n, ne,
                 w,  s,  e,
                sw,  s, se,
            }}),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {{
                0, 0, 0,
                0, 0, 0,
                0, 1, 0,
            }}));
}


BOOST_AUTO_TEST_CASE(converge)
{
    // +---+---+---+   +---+---+---+
    // | 🡦 | 🡣 | 🡧 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡢 | 🡣 | 🡠 | → | 0 | 5 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡢 | 🡣 | 🡠 |   | 0 | 3 | 0 |
    // +---+---+---+   +---+---+---+
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {{
                se,  s, sw,
                 e,  s,  w,
                 e,  s,  w,
            }}),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {{
                0, 0, 0,
                0, 5, 0,
                0, 3, 0,
            }}));
}


BOOST_AUTO_TEST_CASE(parallel_south)
{
    // +---+---+---+   +---+---+---+
    // | 🡣 | 🡣 | 🡣 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡣 | 🡣 | 🡣 | → | 1 | 1 | 1 |
    // +---+---+---+   +---+---+---+
    // | 🡣 | 🡣 | 🡣 |   | 1 | 1 | 1 |
    // +---+---+---+   +---+---+---+

    Shape const array_shape{{9, 9}};
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
            }),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {
                {
                    0, 0, 0,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 0, 0,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 0, 0,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_south_west)
{
    // +---+---+---+   +---+---+---+
    // | 🡧 | 🡧 | 🡧 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡧 | 🡧 | 🡧 | → | 1 | 1 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡧 | 🡧 | 🡧 |   | 1 | 1 | 0 |
    // +---+---+---+   +---+---+---+

    Shape const array_shape{{9, 9}};
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
            }),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {
                {
                    0, 0, 0,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 0, 0,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 0, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_west)
{
    // +---+---+---+   +---+---+---+
    // | 🡠 | 🡠 | 🡠 |   | 1 | 1 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡠 | 🡠 | 🡠 | → | 1 | 1 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡠 | 🡠 | 🡠 |   | 1 | 1 | 0 |
    // +---+---+---+   +---+---+---+

    Shape const array_shape{{9, 9}};
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
            }),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_north_west)
{
    // +---+---+---+   +---+---+---+
    // | 🡤 | 🡤 | 🡤 |   | 1 | 1 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡤 | 🡤 | 🡤 | → | 1 | 1 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡤 | 🡤 | 🡤 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+

    Shape const array_shape{{9, 9}};
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
            }),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    0, 0, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    0, 0, 0,
                },
                {
                    1, 1, 0,
                    1, 1, 0,
                    0, 0, 0,
                },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_north)
{
    // +---+---+---+   +---+---+---+
    // | 🡡 | 🡡 | 🡡 |   | 1 | 1 | 1 |
    // +---+---+---+   +---+---+---+
    // | 🡡 | 🡡 | 🡡 | → | 1 | 1 | 1 |
    // +---+---+---+   +---+---+---+
    // | 🡡 | 🡡 | 🡡 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+

    Shape const array_shape{{9, 9}};
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
            }),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    0, 0, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    0, 0, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    0, 0, 0,
                },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_north_east)
{
    // +---+---+---+   +---+---+---+
    // | 🡥 | 🡥 | 🡥 |   | 0 | 1 | 1 |
    // +---+---+---+   +---+---+---+
    // | 🡥 | 🡥 | 🡥 | → | 0 | 1 | 1 |
    // +---+---+---+   +---+---+---+
    // | 🡥 | 🡥 | 🡥 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+

    Shape const array_shape{{9, 9}};
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
            }),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 0, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    0, 0, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    0, 0, 0,
                },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_east)
{
    // +---+---+---+   +---+---+---+
    // | 🡢 | 🡢 | 🡢 |   | 0 | 1 | 1 |
    // +---+---+---+   +---+---+---+
    // | 🡢 | 🡢 | 🡢 | → | 0 | 1 | 1 |
    // +---+---+---+   +---+---+---+
    // | 🡢 | 🡢 | 🡢 |   | 0 | 1 | 1 |
    // +---+---+---+   +---+---+---+

    Shape const array_shape{{9, 9}};
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
            }),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_south_east)
{
    // +---+---+---+   +---+---+---+
    // | 🡦 | 🡦 | 🡦 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡦 | 🡦 | 🡦 | → | 0 | 1 | 1 |
    // +---+---+---+   +---+---+---+
    // | 🡦 | 🡦 | 🡦 |   | 0 | 1 | 1 |
    // +---+---+---+   +---+---+---+

    Shape const array_shape{{9, 9}};
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
            }),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {
                {
                    0, 0, 0,
                    0, 1, 1,
                    0, 1, 1,
                },
                {
                    0, 0, 0,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 0, 0,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
            }));
}


BOOST_AUTO_TEST_CASE(partitioned)
{
    Shape const array_shape{{9, 9}};

    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                     n,  n,  n,
                     n,  n,  n,
                     n,  n,  n,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                     w,  w,  w,
                     w,  w,  w,
                     w,  w,  w,
                },
                {
                     p,  p,  p,
                     p,  p,  p,
                     p,  p,  p,
                },
                {
                     e,  e,  e,
                     e,  e,  e,
                     e,  e,  e,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                     s,  s,  s,
                     s,  s,  s,
                     s,  s,  s,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
            }),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {
                {
                    1, 1, 0,
                    1, 1, 0,
                    0, 0, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    0, 0, 0,
                },
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 0, 0,
                },
                {
                    1, 1, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                },
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 1, 1,
                },
                {
                    0, 0, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
                {
                    0, 0, 0,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 0, 0,
                    0, 1, 1,
                    0, 1, 1,
                },
            }));
}


BOOST_AUTO_TEST_CASE(all_no_data)
{
    // +---+---+---+   +---+---+---+
    // | X | X | X |   | X | X | X |
    // +---+---+---+   +---+---+---+
    // | X | X | X |   | X | X | X |
    // +---+---+---+   +---+---+---+
    // | X | X | X |   | X | X | X |
    // +---+---+---+   +---+---+---+

    auto const flow_direction =
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {{
                nd, nd, nd,
                nd, nd, nd,
                nd, nd, nd,
            }});
    auto const x{lue::policy::no_data_value<CountElement>};
    auto const inflow_count_we_want =
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {{
                x, x, x,
                x, x, x,
                x, x, x,
            }});

    {
        test_inflow_count(
            lue::policy::inflow_count::DefaultValuePolicies<CountElement, FlowDirectionElement>{},
            flow_direction,
            inflow_count_we_want);
    }

    {
        test_inflow_count3(
            lue::policy::inflow_count3::DefaultValuePolicies<CountElement, FlowDirectionElement>{},
            flow_direction,
            inflow_count_we_want);
    }
}


BOOST_AUTO_TEST_CASE(no_data)
{
    Shape const array_shape{{3, 6}};

    auto const flow_direction =
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {
                {
                     e,  s, nd,
                     e,  s, nd,
                     e,  s, nd,
                },
                {
                     e,  e,  e,
                     e,  e,  e,
                     e,  e,  e,
                },
            });
    auto const x{lue::policy::no_data_value<CountElement>};
    auto const inflow_count_we_want =
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {
                {
                    0, 1, x,
                    0, 2, x,
                    0, 2, x,
                },
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 1, 1,
                },
            });

    {
        test_inflow_count(
            lue::policy::inflow_count::DefaultValuePolicies<CountElement, FlowDirectionElement>{},
            flow_direction,
            inflow_count_we_want);
    }

    {
        test_inflow_count3(
            lue::policy::inflow_count3::DefaultValuePolicies<CountElement, FlowDirectionElement>{},
            flow_direction,
            inflow_count_we_want);
    }
}


BOOST_AUTO_TEST_CASE(merging_inter_partition_streams)
{
    Shape const array_shape{{9, 9}};

    auto const flow_direction =
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {
                { // 0, 0
                    nd, nd, nd,
                    nd, nd, nd,
                    nd, nd, se,
                },
                { // 0, 1
                    nd, nw, se,
                    nd,  s,  e,
                    nd,  s, nd,
                },
                { // 0, 2
                    nd,  s, sw,
                    se,  s,  w,
                    nd,  s, sw,
                },
                { // 1, 0
                    nd, nd,  e,
                    nd, nd, ne,
                    nd, nd,  e,
                },
                { // 1, 1
                     e,  s,  w,
                    nd,  s,  w,
                     e,  s,  w,
                },
                { // 1, 2
                     w,  w, nd,
                     w, nd, nd,
                     w, nd, nd,
                },
                { // 2, 0
                    nd, nd, nd,
                    nd, nd, nd,
                    nd, nd,  s,
                },
                { // 2, 1
                    nd,  s, nd,
                    sw,  w,  w,
                     w,  w,  w,
                },
                { // 2, 2
                    nw, nd, nd,
                     w, nd, nd,
                     w, nd, nd,
                },
            });

    auto const x{lue::policy::no_data_value<CountElement>};
    auto const inflow_count_we_want =
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {
                {
                    x, x, x,
                    x, x, x,
                    x, x, 0,
                },
                {
                    x, 0, 0,
                    x, 0, 0,
                    x, 1, x,
                },
                {
                    x, 0, 0,
                    2, 3, 0,
                    x, 2, 0,
                },
                {
                    x, x, 0,
                    x, x, 0,
                    x, x, 0,
                },
                {
                    3, 3, 1,
                    x, 2, 1,
                    1, 3, 2,
                },
                {
                    1, 2, x,
                    0, x, x,
                    0, x, x,
                },
                {
                    x, x, x,
                    x, x, x,
                    x, x, 2,
                },
                {
                    x, 1, x,
                    1, 2, 1,
                    1, 1, 1,
                },
                {
                    0, x, x,
                    0, x, x,
                    0, x, x,
                },
            });

    {
        test_inflow_count(
            lue::policy::inflow_count::DefaultValuePolicies<CountElement, FlowDirectionElement>{},
            flow_direction,
            inflow_count_we_want);
    }

    {
        test_inflow_count3(
            lue::policy::inflow_count3::DefaultValuePolicies<CountElement, FlowDirectionElement>{},
            flow_direction,
            inflow_count_we_want);
    }
}


BOOST_AUTO_TEST_CASE(spiral_in_case)
{
    Shape const array_shape{{9, 9}};
    Shape const partition_shape{{3, 3}};

    test_inflow_count(
        lue::test::spiral_in(),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {
                { 0, 1, 1,
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
            }));
}
