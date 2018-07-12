#include "lue/object/universes.hpp"
#include "../core/collection.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_universe(
    py::module& module)
{

    BASE_COLLECTION(Universes, Universe)


    py::class_<Universes, UniverseCollection>(
        module,
        "Universes",
        R"(
    Collection of LUE universes

    Zero of more Universe instances can be stored in a Universe
    collection.

    It is not possible to create Universes instances from scratch:
    a constructor is not defined. Universe collections can be obtained
    from Dataset instances.
)")

        .def(
            "add",
            &Universes::add,
            R"(
    Add new universe to collection

    :param str name: Name of universe to create
    :raises RuntimeError: In case the universe cannot be created
)",
            "name"_a,
            py::return_value_policy::reference_internal)

        ;


    py::class_<Universe, hdf5::Group>(
        module,
        "Universe",
        R"(
    LUE universe representing a system's state

    A LUE universe contains phenomena that have something in common,
    for example:

    - They are all generated by the same model
    - They are all measured in the same field campaign
    - They are all created by the same organization

    Universes can be used to organize alternative versions of the same
    state, for example, to manage state generated by different models.
)")

        .def(
            "add_phenomenon",
            &Universe::add_phenomenon,
            R"(
    Add new phenomenon to dataset

    :param str name: Name of phenomenon to create
    :raises RuntimeError: In case the phenomenon cannot be created
)",
            "name"_a,
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "phenomena",
            py::overload_cast<>(&Universe::phenomena),
            R"(
    Return phenomena collection

    :rtype: lue.Phenomena
)",
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace lue
