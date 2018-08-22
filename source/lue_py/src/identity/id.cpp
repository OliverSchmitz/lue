#include "lue/info/identity/id.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_id(
    py::module& module)
{

    py::class_<info::ID, same_shape::Value>(
        module,
        "ID",
        R"(
    ID docstring...
)")

        ;

}

}  // namespace lue