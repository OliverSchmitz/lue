#pragma once
#include "lue/time/unit.hpp"


namespace lue {
namespace time {

/*!
    @brief      Class for representing the length of a tick period
    @tparam     Unit Unit of time

    A TickPeriod instance is represented by a number of time units. This
    determines the resolution of a Duration, Clock and of the TimePoint
    s with which locations in time can be represented.
*/
template<
    typename Unit>
class TickPeriod
{

public:

    using Count = std::size_t;

                   TickPeriod          (Count const nr_units);

                   TickPeriod          (TickPeriod const&)=default;

                   TickPeriod          (TickPeriod&&)=default;

                   ~TickPeriod         ()=default;

    TickPeriod&    operator=           (TickPeriod const&)=default;

    TickPeriod&    operator=           (TickPeriod&&)=default;

    Count          nr_units            () const;

private:

    //! Number of Unit units per tick
    Count          _nr_units;

};


template<
    typename Unit>
inline TickPeriod<Unit>::TickPeriod(
    Count const nr_units)

    : _nr_units{nr_units}

{
}


template<
    typename Unit>
inline typename TickPeriod<Unit>::Count TickPeriod<Unit>::nr_units() const
{
    return _nr_units;
}

}  // namespace time
}  // namespace lue
