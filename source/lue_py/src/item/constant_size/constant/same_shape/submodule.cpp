#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_size {
namespace constant {
namespace same_shape {

void init_collection(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "same_shape",
        R"(
    TODO Docstring...
)");

    init_collection(submodule);
}

}  // namespace same_shape
}  // namespace constant
}  // namespace constant_size
}  // namespace lue
