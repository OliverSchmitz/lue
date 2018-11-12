#include "submodule.hpp"
#include "lue/object/property/same_shape/properties.hpp"
#include "../core/collection.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace same_shape {

void init_properties(
    py::module& module)
{

    BASE_COLLECTION(Properties, Property)

    py::class_<Properties, PropertyCollection>(
        module,
        "Properties",
        R"(
    Properties docstring...
)")

        ;

}

}  // namespace same_shape
}  // namespace lue
