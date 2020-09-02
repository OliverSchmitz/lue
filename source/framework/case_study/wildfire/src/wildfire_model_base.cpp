#include "wildfire_model_base.hpp"
#include "lue/framework/algorithm.hpp"


namespace lue {

WildfireModelBase::WildfireModelBase():

    WildfireModelBase{std::make_shared<NominalRaster>()}

{
}


WildfireModelBase::WildfireModelBase(
    NominalRasterPtr const& state):

    Model{},
    _fire{},
    _burning{},
    _ignite_probability{},
    _spot_ignite_probability{},
    _fire_age{},
    _state_ptr{state}

{
}


WildfireModelBase::BooleanRaster const& WildfireModelBase::fire() const
{
    return _fire;
}


WildfireModelBase::BooleanRaster const& WildfireModelBase::burning() const
{
    return _burning;
}


WildfireModelBase::ScalarRaster const& WildfireModelBase::ignite_probability() const
{
    return _ignite_probability;
}


WildfireModelBase::ScalarRaster const& WildfireModelBase::spot_ignite_probability() const
{
    return _spot_ignite_probability;
}


WildfireModelBase::CountRaster const& WildfireModelBase::fire_age() const
{
    return _fire_age;
}


void WildfireModelBase::initialize()
{
    // Call functions of the specialization to initialize the modelled
    // state. Different specializations do this differently.

    // Initial burning locations: area that is burning or has burnt down
    _fire = initial_fire();

    // Actually burning
    _burning = copy(_fire);

    // Create two areas that differ in how fast they catch fire
    // TODO Use round distribution of weights
    Radius const kernel_radius{25};  // 51x51
    Kernel const kernel = lue::box_kernel<double, 2>(kernel_radius, 1.0);

    // ScalarRaster meh = uniform(_burning, 0.0, 1.0);
    // ScalarRaster mah = focal_mean(meh, kernel);
    // BooleanRaster mih = mah < 0.5;

    BooleanRaster burnability = focal_mean(uniform(_burning, 0.0, 1.0), kernel) < 0.5;

    // Assign probabilities for catching fire
    _ignite_probability = where(burnability, 0.05, 0.01);

    // Assign probabilities for jump fire: we use a linear relation with
    // _ignite_probability, but lower
    _spot_ignite_probability = _ignite_probability / 50.0;

    _fire_age = array_like<CountElement>(_fire, 0);
}


void WildfireModelBase::simulate(
    Count const /* time_step */)
{
    // Find pixels where at least one neighbour is burning and that
    // themselves are not yet burning or burnt down
    KernelShape kernel_shape{3, 3};
    Kernel kernel{
            kernel_shape,
            std::initializer_list<double>{
                    0.0, 1.0, 0.0,
                    1.0, 1.0, 1.0,
                    0.0, 1.0, 0.0
                }
        };

    // TODO Would be useful to be able to pass in output element type
    //     to focal_sum. That would get rid of the cast.
    BooleanRaster cells_not_burning_surrounded_by_fire =
        focal_sum(cast<CountElement>(_burning), kernel) > 0u && !_fire;

    // Select cells that catch new fire from direct neighbours
    BooleanRaster const new_fire =
        cells_not_burning_surrounded_by_fire &&
        uniform(_fire, 0.0, 1.0) < _ignite_probability;

    // Find pixels that have not burned down or at fire and that have
    // fire pixels over a distance (jump dispersal). This should be
    // a round window preferable, diameter I do not know
    // FIXME: round distribution of kernel weights, cirkel_kernel
    Radius const kernel_radius{2};  // 5x5
    kernel = box_kernel<double, 2>(kernel_radius, 1.0);
    BooleanRaster const jump_cells =
        // FIXME 0.5 -> 0.0
        focal_sum(cast<CountElement>(_burning), kernel) > 0u && !_fire;

    // Select cells that catch new fire by jumping fire
    BooleanRaster const new_fire_jump =
        jump_cells && uniform(_fire, 0.0, 1.0) < _spot_ignite_probability;

    _fire = _fire || new_fire || new_fire_jump;

    // Age of fire in timesteps
    _fire_age = where(_fire, _fire_age + 1u, _fire_age);

    // Burning cells
    _burning = _fire && _fire_age < 30u;

    state() = where(_burning, 1, where(_fire, 2, 3));
}

}  // namespace lue
