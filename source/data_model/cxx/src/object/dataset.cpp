#include "lue/object/dataset.hpp"
#include "lue/configure.hpp"
#include <cassert>
#include <stdexcept>


namespace lue {
namespace data_model {

Dataset::Dataset(
    std::string const& name,
    unsigned int const flags,
    AccessPropertyList const& access_property_list):

    hdf5::File{name, flags, access_property_list},
    _universes{*this},
    _phenomena{*this}

{
}


Dataset::Dataset(
    std::string const& name,
    AccessPropertyList const& access_property_list):

    Dataset{name, H5F_ACC_RDONLY, access_property_list}

{
}


/*!
    @brief      Open dataset
    @param      name Name of dataset
    @param      flags File access flags: H5F_ACC_RDWR, H5F_ACC_RDONLY
    @exception  std::runtime_error In case the dataset cannot be opened

    It is assumed that the dataset already exists.
*/
Dataset::Dataset(
    std::string const& name,
    unsigned int const flags):

    Dataset{name, flags, AccessPropertyList{}}

{
}


Dataset::Dataset(
    hdf5::File&& file):

    hdf5::File{std::forward<hdf5::File>(file)},
    _universes{*this},
    _phenomena{*this}

{
}


std::string Dataset::lue_version() const
{
    return attribute<std::string>("lue_version");
}


/*!
    @brief      Add new universe to dataset
    @sa         Universes::add()
*/
Universe& Dataset::add_universe(
    std::string const& name)
{
    return _universes.add(name);
}


/*!
    @brief      Add new phenomenon to dataset
    @sa         Phenomena::add()
*/
Phenomenon& Dataset::add_phenomenon(
    std::string const& name)
{
    return _phenomena.add(name);
}


/*!
    @brief      return universes collection.
    @warning    a universes collection must be present
*/
Universes const& Dataset::universes() const
{
    return _universes;
}


/*!
    @brief      return universes collection.
    @warning    a universes collection must be present
*/
Universes& Dataset::universes()
{
    return _universes;
}


/*!
    @brief      Return phenomena collection
    @warning    A phenomena collection must be present
*/
Phenomena const& Dataset::phenomena() const
{
    return _phenomena;
}


/*!
    @brief      Return phenomena collection
    @warning    A phenomena collection must be present
*/
Phenomena& Dataset::phenomena()
{
    return _phenomena;
}


/*!
    @brief      Return whether or not a dataset exists
    @param      name Name of dataset
    @warning    This function only checks whether a regular file named
                @a name is present. No attempt is made to verify the file
                is accessible or is formatted correctly.
*/
bool dataset_exists(
    std::string const& name)
{
    return hdf5::file_exists(name);
}


namespace {

Dataset create_dataset(
    hdf5::File&& file)
{
    create_universes(file);
    create_phenomena(file);

    file.attributes().write<std::string>("lue_version", LUE_VERSION);

    return Dataset{std::move(file)};
}

}  // Anonymous namespace


/*!
    @brief      Create dataset
    @param      name Name of dataset
    @return     Dataset instance
    @exception  std::runtime_error In case the dataset cannot be created
*/
Dataset create_dataset(
    std::string const& name)
{
    auto file = hdf5::create_file(name);

    return create_dataset(std::move(file));
}


Dataset create_in_memory_dataset(
    std::string const& name)
{
    auto file = hdf5::create_in_memory_file(name);

    return create_dataset(std::move(file));
}


/*!
    @brief      Remove dataset
    @param      name Name of dataset
    @exception  std::runtime_error In case the dataset cannot be removed
*/
void remove_dataset(
    std::string const& name)
{
    hdf5::remove_file(name);
}

} // namespace data_model
} // namespace lue
