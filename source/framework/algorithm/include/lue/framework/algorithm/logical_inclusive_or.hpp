#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"


namespace lue {
    namespace detail {

        template<
            typename InputElement,
            typename OutputElement_=std::uint8_t>
        class LogicalInclusiveOr
        {

        public:

            using OutputElement = OutputElement_;

            constexpr OutputElement operator()(
                InputElement const& input_element1,
                InputElement const& input_element2) const noexcept
            {
                return input_element1 || input_element2;
            }

        };

    }  // namespace detail


    namespace policy::logical_inclusive_or {

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::logical_inclusive_or


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS(logical_inclusive_or, detail::LogicalInclusiveOr)

}  // namespace lue
