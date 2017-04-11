#pragma once
#include "lue/constant_size/time/omnipresent/different_shape/property.h"
#include "lue/constant_size/time/omnipresent/same_shape/property.h"
#include "lue/constant_size/time/omnipresent/property_set.h"
#include "lue/dataset.h"


namespace lue {
namespace hl {

namespace omnipresent = constant_size::time::omnipresent;


class Raster
{

public:

    class Discretization
    {

    public:

                   Discretization      (hsize_t const nr_rows,
                                        hsize_t const nr_cols);

        hsize_t const* shape           () const;

        hsize_t    nr_rows             () const;

        hsize_t    nr_cols             () const;

    private:

        hsize_t const _shape[2];

    };


    class Domain
    {

    public:

                   Domain              (std::string const& crs,
                                        double const west,
                                        double const north,
                                        double const east,
                                        double const south);

        double const*  coordinates     () const;

    private:

        std::string const _crs;

        double const _coordinates[4];

    };


    class Band
    {

    public:

                   Band                (omnipresent::different_shape::
                                            Property&& property);

        void       write               (hdf5::Dataspace const& memory_dataspace,
                                        hdf5::Offset const& start,
                                        hdf5::Stride const& stride,
                                        hdf5::Count const& count,
                                        void const* buffer);

    private:

        omnipresent::different_shape::Property _property;

    };

                   Raster              (Dataset&& dataset,
                                        omnipresent::PropertySet&& property_set,
                                        omnipresent::same_shape::Property&&
                                            discretization_property,
                                        Discretization const& discretization);

                   Raster              (Raster const& other)=delete;

                   Raster              (Raster&& other)=default;

                   ~Raster             ()=default;

    Raster&        operator=           (Raster const& other)=delete;

    Raster&        operator=           (Raster&& other)=default;

    Band           add_band            (std::string const& name,
                                        hdf5::Datatype const& datatype);

private:

    Dataset        _dataset;

    omnipresent::PropertySet _property_set;

    omnipresent::same_shape::Property _discretization_property;

    Discretization _discretization;

};


Raster             create_raster       (std::string const& dataset_name,
                                        std::string const& phenomenon_name,
                                        std::string const& property_set_name,
                                        Raster::Domain const& domain,
                                        Raster::Discretization const&
                                            discretization);

}  // namespace hl
}  // namespace lue
