#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_size {
namespace constant {

// Implementation in item/ tree
void init_submodule(py::module& module);

}  // namespace constant


namespace variable {

// Implementation in item/ tree
void init_submodule(py::module& module);

}  // namespace variable


namespace time {

void init_submodule(py::module& module);

}  // namespace time


void init_domain_class(py::module& module);
void init_property_class(py::module& module);
void init_property_set_class(py::module& module);
void init_space_domain_class(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "constant_size",
        R"(
    Module implementing the API for property sets with a constant number
    of items

    .. automodule:: lue.constant_size.constant
    .. automodule:: lue.constant_size.time
)");
    init_domain_class(submodule);
    init_space_domain_class(submodule);
    init_property_class(submodule);
    init_property_set_class(submodule);

    constant::init_submodule(submodule);
    variable::init_submodule(submodule);
    time::init_submodule(submodule);
}

}  // namespace constant_size
}  // namespace lue
