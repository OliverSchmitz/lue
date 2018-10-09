#include "lue/object/space/space_domain.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_space_domain(
    py::module& module)
{

    py::enum_<Mobility>(
        module,
        "Mobility",
        R"(
    Objects can be stationary in space or mobile
)")
        .value("mobile", Mobility::mobile)
        .value("stationary", Mobility::stationary)
        ;


    py::enum_<SpaceDomainItemType>(
        module,
        "SpaceDomainItemType",
        R"(
    Locations in space can be represented by boxes or points
)")
        .value("box", SpaceDomainItemType::box)
        .value("point", SpaceDomainItemType::point)
        ;


    py::class_<SpaceConfiguration>(
        module,
        "SpaceConfiguration",
        R"(
    Class for representing the aspects of the spatial domain
)")

        .def(
            py::init<Mobility, SpaceDomainItemType>())

        ;

    py::class_<SpaceDomain, hdf5::Group>(
        module,
        "SpaceDomain",
        R"(
    TODO
)")

        .def(
            "set_presence_discretization",
            &SpaceDomain::set_presence_discretization)

        .def_property_readonly(
            "presence_is_discretized",
            &SpaceDomain::presence_is_discretized)

        .def_property_readonly(
            "discretized_presence_property",
            &SpaceDomain::discretized_presence_property)

        .def_property_readonly(
            "value",
            [](
                SpaceDomain& space_domain)
            {
                py::object collection = py::none();

                auto const& configuration = space_domain.configuration();

                switch(configuration.value<Mobility>()) {
                    case Mobility::stationary: {
                        switch(configuration.value<SpaceDomainItemType>()) {
                            case SpaceDomainItemType::point: {
                                using Value = StationarySpacePoint;
                                collection = py::cast(new Value(
                                    space_domain.value<Value>()));
                                break;
                            }
                            case SpaceDomainItemType::box: {
                                using Value = StationarySpaceBox;
                                collection = py::cast(new Value(
                                    space_domain.value<Value>()));
                                break;
                            }
                        }

                        break;
                    }
                    case Mobility::mobile: {
                        switch(configuration.value<SpaceDomainItemType>()) {
                            case SpaceDomainItemType::point: {
                                using Value = MobileSpacePoint;
                                collection = py::cast(new Value(
                                    space_domain.value<Value>()));
                                break;
                            }
                            case SpaceDomainItemType::box: {
                                using Value = MobileSpaceBox;
                                collection = py::cast(new Value(
                                    space_domain.value<Value>()));
                                break;
                            }
                        }

                        break;
                    }
                }

                return collection;
            },
            R"(
    Return instance representing the collection of space domain items
)")

        ;

}

}  // namespace lue