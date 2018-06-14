#include "lue/item/different_shape/constant_shape/synchronous_value.hpp"
#include "lue/item/item_configuration.hpp"


namespace lue {
namespace different_shape {
namespace constant_shape {

/*!
    @brief      Open collection @a name in @a parent
*/
SynchronousValue::SynchronousValue(
    hdf5::Group const& parent,
    std::string const& name)

    : Value{parent, name}

{
}


/*!
    @brief      Open collection @a name in @a parent
*/
SynchronousValue::SynchronousValue(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : Value{parent, name, memory_datatype}

{
}


/*!
    @brief      Move in @a group
*/
SynchronousValue::SynchronousValue(
    hdf5::Group&& group,
    hdf5::Datatype const& memory_datatype)

    : Value{std::forward<hdf5::Group>(group), memory_datatype}

{
}


/*!
    @brief      Create collection @a name in @a parent

    The datatype is of the individual values. The @a rank passed in
    defines the dimensionality of the item values.
*/
SynchronousValue create_synchronous_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{
    return create_synchronous_value(
        parent, name, hdf5::file_datatype(memory_datatype), memory_datatype,
        rank);
}


/*!
    @brief      Create collection @a name in @a parent

    The datatypes are of the individual values. The @a rank passed in
    defines the dimensionality of the item values.
*/
SynchronousValue create_synchronous_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{
    auto value = create_value(
        parent, name, file_datatype, memory_datatype, rank);

    ItemConfiguration{
        ShapePerItem::different,
        Occurrence::synchronous,
        ShapeVariability::constant,
        CollectionVariability::variable
    }.save(value.attributes());

    return SynchronousValue{std::move(value), memory_datatype};
}

}  // namespace constant_shape
}  // namespace different_shape
}  // namespace lue