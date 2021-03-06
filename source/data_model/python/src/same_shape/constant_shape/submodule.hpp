#pragma once


namespace pybind11 {

class module;

}  // namespace pybind11


namespace lue {
namespace data_model {
namespace same_shape {
namespace constant_shape {

void               init_properties     (pybind11::module& module);

void               init_property       (pybind11::module& module);

void               init_submodule      (pybind11::module& module);

void               init_value          (pybind11::module& module);

}  // namespace constant_shape
}  // namespace same_shape
}  // namespace data_model
}  // namespace lue
