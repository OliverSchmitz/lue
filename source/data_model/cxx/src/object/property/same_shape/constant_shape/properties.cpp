#include "lue/object/property/same_shape/constant_shape/properties.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace data_model {
namespace same_shape {
namespace constant_shape {

Properties::Properties(
    hdf5::Group& parent):

    Collection<Property>{parent, same_shape_constant_shape_tag}

{
}


Properties::Properties(
    Collection<Property>&& collection):

    Collection<Property>{std::forward<Collection<Property>>(collection)}

{
}


Property& Properties::add(
    std::string const& name,
    hdf5::Datatype const& datatype)
{
    return Collection::add(name, create_property(*this, name, datatype));
}


Property& Properties::add(
    std::string const& name,
    hdf5::Datatype const& datatype,
    hdf5::Shape const& shape)
{
    return Collection::add(
        name, create_property(*this, name, datatype, shape));
}


Properties create_properties(
    hdf5::Group& parent)
{
    auto collection = create_collection<Property>(
        parent, same_shape_constant_shape_tag);

    return Properties{std::move(collection)};
}

}  // namespace constant_shape
}  // namespace same_shape
}  // namespace data_model
}  // namespace lue
