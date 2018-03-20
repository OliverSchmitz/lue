#include "lue/space/mobile/constant_collection/synchronous_point.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace mobile {
namespace constant_collection {

SynchronousPoint::SynchronousPoint(
    hdf5::Group const& parent,
    hdf5::Datatype const& memory_datatype)

    : same_shape::constant_shape::constant_collection::SynchronousValue{
        parent, coordinates_tag, memory_datatype}

{
}


SynchronousPoint create_synchronous_point(
    hdf5::Group& parent,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    return create_synchronous_point(
        parent, file_datatype(memory_datatype), memory_datatype, rank);
}


SynchronousPoint create_synchronous_point(
    hdf5::Group& parent,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    // A point is defined by the coordinates along each dimension
    hdf5::Shape value_shape = { rank };

    same_shape::constant_shape::constant_collection::create_synchronous_value(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return SynchronousPoint{std::move(parent), memory_datatype};
}

}  // namespace constant_collection
}  // namespace mobile
}  // namespace lue