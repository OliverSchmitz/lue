#pragma once
// #include "lue/framework/core/array_partition_data.hpp"
// #include "lue/framework/core/array_partition_definition.hpp"
// #include <boost/multi_array.hpp>
#include "lue/framework/core/component/partitioned_array.hpp"
#include <boost/range/irange.hpp>
#include <experimental/iterator>


// namespace boost {
// 
// template<
//     typename Element,
//     Rank rank,
//     typename Allocator>
// std::ostream& operator<<(
//     std::ostream& stream,
//     multi_array<Element, rank, Allocator> const& array)
// {
//     stream << '[';
//     auto joiner = std::experimental::make_ostream_joiner(stream, ", ");
// 
//     auto const size = array.num_elements();
//     auto const begin = array.data();
//     auto const end = array.data() + size;
// 
//     // Max number of values to print at start and end
//     std::size_t const halo = 5;
// 
//     if(size <= 2 * halo) {
//         // Print all values
//         std::copy(begin, end, joiner);
//     }
//     else {
//         // Print first and last halo number of values
//         std::copy(begin, begin + halo, joiner);
//         stream << ", ..., ";
//         std::copy(end - halo, end, joiner);
//     }
//     stream << ']';
// 
//     return stream;
// }
// 
// }  // namespace boost


namespace std {

template<
    typename... Ts>
std::ostream& operator<<(
    std::ostream& stream,
    std::tuple<Ts...> const& tuple)
{
    stream << '(';
    std::apply(

            [&stream](
                auto&&... ts)
            {
                ((stream << ts << ", "), ...);
            },

        tuple);
    stream << ')';

    return stream;
}


// template<
//     typename T,
//     lue::Rank rank>
// std::ostream& operator<<(
//     std::ostream& stream,
//     std::array<T, rank> const& array)
// {
//     stream << '[';
//     std::copy(
//         std::begin(array), std::end(array),
//         std::experimental::make_ostream_joiner(stream, ", "));
// 
//         // Prints a separator after the last element...
//         // std::ostream_iterator<Index>(stream, ", "));
//     stream << ']';
// 
//     return stream;
// }


template<
    typename Element,
    lue::Rank rank>
std::ostream& stream_span(
    std::ostream& stream,
    lue::DynamicSpan<Element, rank> const& span)
{
    auto const idxs = boost::irange<lue::Index>(0, rank);

    stream << '[';

    std::transform(
        std::begin(idxs), std::end(idxs),
        std::experimental::make_ostream_joiner(stream, ", "),

        [&span](lue::Index const idx)
        {
            return span.extent(idx);
        });

    stream << ']';

    return stream;
}


template<
    typename Element>
std::ostream& operator<<(
    std::ostream& stream,
    lue::DynamicSpan<Element, 1> const& span)
{
    return stream_span<Element, 1>(stream, span);
}


template<
    typename Element>
std::ostream& operator<<(
    std::ostream& stream,
    lue::DynamicSpan<Element, 2> const& span)
{
    return stream_span<Element, 2>(stream, span);
}

}  // namespace std


namespace lue {

template<
    typename Index,
    Rank rank>
std::ostream& operator<<(
    std::ostream& stream,
    ArrayPartitionDefinition<Index, rank> const& definition)
{
    stream <<
        "start: " << definition.start() << ", "
        "shape: " << definition.shape();

    return stream;
}


template<
    typename Element,
    Rank rank>
std::ostream& operator<<(
    std::ostream& stream,
    ArrayPartitionData<Element, rank> const& data)
{
    stream
        << data.shape()
        << ", "
        << "[\n";

    auto joiner = std::experimental::make_ostream_joiner(stream, ", ");

    auto const nr_elements = data.nr_elements();
    auto const begin = data.begin();
    auto const end = data.end();

    // Max number of values to print at start and end
    Count const halo = 15;

    if(nr_elements <= 2 * halo) {
        // Print all values
        std::copy(begin, end, joiner);
    }
    else {
        // Print first and last halo number of values
        std::copy(begin, begin + halo, joiner);
        stream << ", ..., ";
        std::copy(end - halo, end, joiner);
    }

    stream << "]\n";

    return stream;
}


template<
    typename Element,
    Rank rank>
std::ostream& operator<<(
    std::ostream& stream,
    ArrayPartition<Element, rank> const& partition)
{
    stream
        << "    shape      : " << partition.shape().get() << "\n"
        << "    nr_elements: " << partition.nr_elements().get() << "\n"
        << partition.data(CopyMode::share).get()
        ;

    return stream;
}


template<
    typename Element,
    Rank rank>
std::ostream& operator<<(
    std::ostream& stream,
    PartitionedArray<Element, rank> const& array)
{
    stream
        << "nr_partitions: " << array.nr_partitions() << "\n"
        << "nr_elements  : " << array.nr_elements() << "\n"
        << "shape        : " << array.shape() << "\n"
        << array.partitions()
        ;

    return stream;
}

}  // namespace lue
