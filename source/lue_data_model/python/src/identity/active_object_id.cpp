#include "../python_extension.hpp"
#include "lue/info/identity/active_object_id.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_active_object_id(
    py::module& module)
{

    py::class_<ActiveObjectID, same_shape::constant_shape::Value>(
        module,
        "ActiveObjectID",
        R"(
    ActiveObjectID docstring...
)")

        .def_property_readonly(
            "nr_ids",
            &ActiveObjectID::nr_ids,
            R"(
    nr_ids docstring...
)")

        ;

}

}  // namespace lue
