#include "lue/item/different_shape/constant_shape/constant_collection/synchronous_value.hpp"
#include "lue/item/constant_shape/collection.hpp"
#include "lue/item/item_configuration.hpp"
#include "lue/tag.hpp"
#include "lue/hdf5/chunk.hpp"


namespace lue {
namespace different_shape {
namespace constant_shape {
namespace constant_collection {

/*!
    @brief      Open collection @a name in @a parent
*/
SynchronousValue::SynchronousValue(
    hdf5::Group const& parent,
    std::string const& name)

    : Group{parent, name},
      _nr_time_domain_items{attributes().read<hsize_t>(
          nr_time_domain_items_tag)},
      _nr_items{attributes().read<hsize_t>(nr_items_tag)},
      _rank{attributes().read<int>(rank_tag)},
      _file_datatype{hdf5::decode_datatype(
          attributes().read<std::vector<unsigned char>>(datatype_tag))},
      _memory_datatype{hdf5::memory_datatype(_file_datatype)}

{
}


/*!
    @brief      Open collection @a name in @a parent
*/
SynchronousValue::SynchronousValue(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : Group{parent, name},
      _nr_time_domain_items{attributes().read<hsize_t>(
          nr_time_domain_items_tag)},
      _nr_items{attributes().read<hsize_t>(nr_items_tag)},
      _rank{attributes().read<int>(rank_tag)},
      _file_datatype{hdf5::decode_datatype(
          attributes().read<std::vector<unsigned char>>(datatype_tag))},
      _memory_datatype{memory_datatype}

{
}


/*!
    @brief      Move in @a group
*/
SynchronousValue::SynchronousValue(
    hdf5::Group&& group,
    hdf5::Datatype const& memory_datatype)

    : Group{std::forward<hdf5::Group>(group)},
      _nr_time_domain_items{attributes().read<hsize_t>(
          nr_time_domain_items_tag)},
      _nr_items{attributes().read<hsize_t>(nr_items_tag)},
      _rank{attributes().read<int>(rank_tag)},
      _file_datatype{hdf5::decode_datatype(
          attributes().read<std::vector<unsigned char>>(datatype_tag))},
      _memory_datatype{memory_datatype}

{
}


/*!
    @brief      Return number of time domain items for which values are stored
*/
hsize_t SynchronousValue::nr_time_domain_items() const
{
    return _nr_time_domain_items;
}


/*!
    @brief      Return number of items for which values are stored
*/
hsize_t SynchronousValue::nr_items() const
{
    return _nr_items;
}


/*!
    @brief      Return rank of each of the item's value
*/
int SynchronousValue::rank() const
{
    return _rank;
}


/*!
    @brief      Return in-file datatype
*/
hdf5::Datatype const& SynchronousValue::file_datatype() const
{
    return _file_datatype;
}


/*!
    @brief      Return in-memory datatype
*/
hdf5::Datatype const& SynchronousValue::memory_datatype() const
{
    return _memory_datatype;
}


/*!
    @brief      Reserve space for a single item's value
    @param      item_idx Index of item
    @param      value_shape Shape of the item's value

    The underlying HDF5 dataset is chunked according to hdf5::chunk_shape().
*/
void SynchronousValue::reserve_value(
    hsize_t const item_idx,
    hsize_t const nr_time_domain_items,
    hdf5::Shape const& value_shape)
{
    // Create dataset with this shape.
    std::string name = std::to_string(item_idx);

    hdf5::Shape dimension_sizes{value_shape};
    dimension_sizes.insert(dimension_sizes.begin(), 1, nr_time_domain_items);

    hdf5::Shape max_dimension_sizes{value_shape};
    max_dimension_sizes.insert(max_dimension_sizes.begin(), 1, H5S_UNLIMITED);

    auto dataspace = hdf5::create_dataspace(
        dimension_sizes, max_dimension_sizes);

    hdf5::Dataset::CreationPropertyList creation_property_list;
    auto chunk_dimension_sizes =
        hdf5::chunk_shape(value_shape, _file_datatype.size());
    creation_property_list.set_chunk(chunk_dimension_sizes);

    auto dataset = hdf5::create_dataset(
        id(), name, _file_datatype, dataspace, creation_property_list);
}


/*!
    @brief      Reserve space for @a nr_time_domain_items and @a nr_items
                item values shaped as @a value_shapes
*/
void SynchronousValue::reserve(
    hsize_t const nr_items,
    hsize_t const nr_time_domain_items,
    hsize_t const* value_shapes)
{
    for(hsize_t i = 0; i < nr_items; ++i) {
        hsize_t const* shape_ptr = &(value_shapes[i * _rank]);
        hdf5::Shape shape(shape_ptr, shape_ptr + _rank);
        reserve_value(i, nr_time_domain_items, shape);
    }

    _nr_time_domain_items = nr_time_domain_items;
    _nr_items = nr_items;

    attributes().write<hsize_t>(nr_time_domain_items_tag, nr_time_domain_items);
    attributes().write<hsize_t>(nr_items_tag, nr_items);
}


/*!
    @brief      Return dataset corresponding to item @a item_idx
*/
Array SynchronousValue::operator[](
    hsize_t const item_idx) const
{
    return Array{*this, std::to_string(item_idx), _memory_datatype};
}


hdf5::Shape SynchronousValue::value_shape(
    hsize_t item_idx)
{
    auto const shape = operator[](item_idx).shape();

    assert(shape.size() >= 1);

    return hdf5::Shape{shape.begin() + 1, shape.end()};
}


hdf5::Hyperslab SynchronousValue::hyperslab(
    hdf5::Shape const& dataset_shape,
    hsize_t const time_idx) const
{
    hdf5::Offset offset(dataset_shape.size(), 0);
    offset[0] = time_idx;

    hdf5::Count count(dataset_shape.begin(), dataset_shape.end());
    count[0] = 1;

    return hdf5::Hyperslab{offset, count};
}


void SynchronousValue::read(
    hsize_t const item_idx,
    void* buffer)
{
    operator[](item_idx).read(buffer);
}


void SynchronousValue::read(
    hsize_t const item_idx,
    hsize_t const time_idx,
    void* buffer)
{
    auto array = operator[](item_idx);

    array.read(hyperslab(array.shape(), time_idx), buffer);
}


void SynchronousValue::write(
    hsize_t const item_idx,
    void const* buffer)
{
    operator[](item_idx).write(buffer);
}


void SynchronousValue::write(
    hsize_t const item_idx,
    hsize_t const time_idx,
    void const* buffer)
{
    auto array = operator[](item_idx);

    array.write(hyperslab(array.shape(), time_idx), buffer);
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
    auto group = lue::constant_shape::create_group(
        parent, name, file_datatype, rank);

    group.attributes().write<hsize_t>(nr_time_domain_items_tag, 0);

    ItemConfiguration{
        ShapePerItem::different,
        Occurrence::synchronous,
        ShapeVariability::constant,
        CollectionVariability::constant
    }.save(group.attributes());

    return SynchronousValue{std::move(group), memory_datatype};
}

}  // namespace constant_collection
}  // namespace constant_shape
}  // namespace different_shape
}  // namespace lue