#pragma once
#include "lue/info/property/property_group.hpp"
#include "lue/info/property/property_traits.hpp"
#include "lue/array/different_shape/variable_shape/value.hpp"


namespace lue {
namespace data_model {
namespace different_shape {
namespace variable_shape {

class Property:
    public PropertyGroup
{

public:

    using Value = different_shape::variable_shape::Value;

                   Property            (hdf5::Group& parent,
                                        std::string const& name);

                   Property            (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   Property            (Property const&)=delete;

                   Property            (Property&&)=default;

                   Property            (PropertyGroup&& group,
                                        Value&& value);

                   ~Property           ()=default;

    Property&      operator=           (Property const&)=delete;

    Property&      operator=           (Property&&)=default;

    Value const&   value               () const;

    Value&         value               ();

private:

    Value          _value;

};


Property           create_property     (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        Rank rank);

Property           create_property     (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        Rank rank);

}  // namespace variable_shape
}  // namespace different_shape


template<>
class PropertyTraits<different_shape::variable_shape::Property>
{

public:

    using Value = different_shape::variable_shape::Property::Value;

};

}  // namespace data_model
}  // namespace lue
