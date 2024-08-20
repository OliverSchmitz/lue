#include "lue/py/framework/algorithm/less_than.hpp"
#include "lue/py/framework/type_traits.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_less_than_overloads(pybind11::module& module)
        {
            Rank const rank{2};

            using OutputElement = std::uint8_t;

            module.def(
                "less_than",
                less_than<
                    OutputElement,
                    rank,
                    PartitionedArray<Element, rank>,
                    PartitionedArray<Element, rank>>);
            module.def(
                "less_than",
                less_than<OutputElement, rank, PartitionedArray<Element, rank>, Scalar<Element>>);
            module.def("less_than", less_than<OutputElement, rank, PartitionedArray<Element, rank>, Element>);

            module.def(
                "less_than",
                less_than<OutputElement, rank, Scalar<Element>, PartitionedArray<Element, rank>>);
            module.def("less_than", less_than<OutputElement, rank, Element, PartitionedArray<Element, rank>>);

            module.def("less_than", less_than<OutputElement, Scalar<Element>, Element>);
            module.def("less_than", less_than<OutputElement, Element, Scalar<Element>>);
        }

    }  // Anonymous namespace


    void bind_less_than(pybind11::module& module)
    {
        define_less_than_overloads<std::uint8_t>(module);
        define_less_than_overloads<std::uint32_t>(module);
        define_less_than_overloads<std::uint64_t>(module);
        define_less_than_overloads<std::int32_t>(module);
        define_less_than_overloads<std::int64_t>(module);
        define_less_than_overloads<float>(module);
        define_less_than_overloads<double>(module);
    }

}  // namespace lue::framework
