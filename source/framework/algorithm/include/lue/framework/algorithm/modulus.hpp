#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy.hpp"

namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_ = InputElement>
        class Modulus
        {
            public:

                static_assert(std::is_integral_v<InputElement>);

                using OutputElement = OutputElement_;

                constexpr auto operator()(
                    InputElement const& input_element1, InputElement const& input_element2) const noexcept -> OutputElement
                {
                    return input_element1 % input_element2;
                }
        };

    }  // namespace detail


    namespace policy::modulus {

        template<typename Element>
        class DomainPolicy
        {

            public:

                static constexpr auto within_domain(
                    [[maybe_unused]] Element const numerator, Element const denominator) noexcept -> bool
                {
                    return denominator != 0;
                }
        };

    }  // namespace policy::modulus


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_SAME_OUTPUT_ELEMENT(modulus, detail::Modulus)

}  // namespace lue
