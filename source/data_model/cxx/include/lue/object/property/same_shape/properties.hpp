#pragma once
#include "lue/info/property/same_shape/property.hpp"
#include "lue/object/property/properties_traits.hpp"
#include "lue/core/collection.hpp"


namespace lue {
namespace data_model {
namespace same_shape {

/*!
    @brief      Collection of same shape x constant value properties
*/
class Properties:
    public Collection<Property>
{

public:

    explicit       Properties          (hdf5::Group& parent);

                   Properties          (Collection<Property>&& collection);

                   Properties          (Properties const&)=delete;

                   Properties          (Properties&&)=default;

                   ~Properties         ()=default;

    Properties&    operator=           (Properties const&)=delete;

    Properties&    operator=           (Properties&&)=default;

    Property&      add                 (std::string const& name,
                                        hdf5::Datatype const& datatype);

    Property&      add                 (std::string const& name,
                                        hdf5::Datatype const& datatype,
                                        hdf5::Shape const& shape);

private:

};


Properties         create_properties   (hdf5::Group& parent);

}  // namespace same_shape


template<>
class PropertyTraits<same_shape::Properties>
{

public:

    using Property = same_shape::Properties::Element;

    using Value = ValueT<Property>;

};

}  // namespace data_model
}  // namespace lue
