#pragma once
#include "lue/info/time/location_in_time.hpp"


namespace lue {
namespace data_model {

/*!
    - Zero or more time boxes per item
    - Each time box has a unique location in time
*/
class TimeBox:
    public LocationInTime
{

public:

                   TimeBox             (hdf5::Group& parent);

                   TimeBox             (TimeBox const&)=delete;

                   TimeBox             (TimeBox&&)=default;

                   TimeBox             (LocationInTime&& value);

                   ~TimeBox            ()=default;

    TimeBox&       operator=           (TimeBox const&)=delete;

    TimeBox&       operator=           (TimeBox&&)=default;

    Count          nr_boxes            () const;

private:

};


TimeBox            create_time_box     (hdf5::Group& parent);


template<>
class ValueTraits<TimeBox>
{

public:

    using Element = typename ValueTraits<LocationInTime>::Element;

};

}  // namespace data_model
}  // namespace lue
