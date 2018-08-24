#include "lue/validate.hpp"
#include "lue/object/dataset.hpp"
#include "lue/hdf5/validate.hpp"
#include <algorithm>
#include <sstream>


namespace lue {
namespace {

void throw_not_supported_yet(
    std::string const& message)
{
    throw std::runtime_error(fmt::format(
        "This feature is not supported yet: {}\n"
        "You may want to open an issue here: "
        "https://github.com/pcraster/lue/issues",
        message));
}


std::string error_message(
    hdf5::Issues const& issues)
{
    std::stringstream stream;

    // Aggregate all issues by id. Print issues per id.
    // The idea is to print important messages first.

    // Collection of ids, ordered by errors.
    std::vector<hdf5::Identifier> ids;

    // Mapping of id to messages.
    std::map<hdf5::Identifier, std::vector<std::string>> messages;

    // Handle all errors. These will be printed first.
    for(auto const& error: issues.errors()) {
        if(messages.find(error.id()) == messages.end()) {
            ids.push_back(error.id());
        }

        messages[error.id()].push_back("error: " + error.message());
    }

    // Handle all warnings. Warnings will be added to the list of messages
    // created above. For objects without errors, the warnings will be
    // stored last.
    for(auto const& warning: issues.warnings()) {
        if(messages.find(warning.id()) == messages.end()) {
            ids.push_back(warning.id());
        }

        messages[warning.id()].push_back("warning: " + warning.message());
    }


    // Print the issues. First the errors (and possibly warnings) are printed,
    // and after that the warnings for objects without errors.
    for(auto const& id: ids) {
        stream << id.pathname() << ":\n";

        for(auto const& message: messages[id]) {
            stream << "- " << message << "\n";
        }
    }

    return stream.str();
}

}  // Anonymous namespace


bool assert_ids_are_unique(
    hdf5::Identifier const& id,
    std::vector<ID> ids,
    hdf5::Issues& issues)
{
    std::sort(ids.begin(), ids.end());

    auto const it = std::adjacent_find(ids.begin(), ids.end());

    bool unique = it == ids.end();

    if(!unique) {
        issues.add_error(id, fmt::format(
            "Object IDs must be unique, but at least one duplicate was found "
            "({})",
            *it
        ));
    }

    return unique;
}


template<
    typename Collection>
bool assert_strictly_increasing(
    hdf5::Identifier const& id,
    Collection const& collection,
    hdf5::Issues& issues)
{
    using Value = typename Collection::value_type;
    bool strictly_increasing = false;

    // Find two adjacent values where the first value is larger or equal to
    // the second. In that case the collection is not strictly increasing.
    auto const it = std::adjacent_find(
        collection.begin(), collection.end(),
        [](Value const& lhs, Value const& rhs) {
            return lhs >= rhs;
        }
    );

    strictly_increasing = it == collection.end();

    if(!strictly_increasing) {
        issues.add_error(id, fmt::format(
            "Values must be strictly increasing, "
            "but at least two adjacent values where not "
            "({} <= {})",
            *it, *(it + 1)
        ));
    }

    return strictly_increasing;
}


void validate(
    ObjectTracker const& object_tracker,
    same_shape::Value const& value,
    hdf5::Issues& issues)
{
    // - Per object array, the object tracker contains an object ID.
    //   The number of object arrays must equal the number of IDs.

    auto const nr_objects = object_tracker.id().nr_objects();
    auto const nr_object_arrays = value.nr_arrays();

    if(nr_object_arrays != nr_objects) {
        issues.add_error(value.id(), fmt::format(
            "Number of object arrays in value array does not equal "
            "the number of IDs in object tracker "
            "({} != {})",
            nr_object_arrays, nr_objects
        ));
    }
}


void validate(
    ObjectTracker const& object_tracker,
    different_shape::Value const& value,
    hdf5::Issues& issues)
{
    // - Per object array, the object tracker contains an object ID.
    //   The number of object arrays (== number of value arrays) must
    //   equal the number of IDs.

    auto const nr_objects = object_tracker.id().nr_objects();
    auto const nr_object_arrays = value.nr_objects();

    if(nr_object_arrays != nr_objects) {
        issues.add_error(value.id(), fmt::format(
            "Number of object arrays in value does not equal "
            "the number of IDs in object tracker "
            "({} != {})",
            nr_object_arrays, nr_objects
        ));
    }
}


void validate(
    ObjectTracker const& object_tracker,
    same_shape::constant_shape::Value const& value,
    hdf5::Issues& issues)
{
    // - Per location in time, the object tracker contains information
    //   about which objects are active. The sum of the sizes of the
    //   these active sets must equal the number of object arrays.
    {
        auto const nr_active_objects = object_tracker.active_id().nr_ids();
        auto const nr_object_arrays = value.nr_arrays();

        if(nr_object_arrays != nr_active_objects) {
            issues.add_error(value.id(), fmt::format(
                "Number of object arrays in value array does not equal "
                "the number of active object IDs in object tracker "
                "({} != {})",
                nr_object_arrays, nr_active_objects
            ));
        }
    }
}


void validate(
    ObjectTracker const& /* object_tracker */,
    different_shape::constant_shape::Value const& /* value */,
    hdf5::Issues& /* issues */)
{
    throw_not_supported_yet(
        "validation of different_shape::constant_shape::Value");
}


void validate(
    ObjectTracker const& object_tracker,
    same_shape::variable_shape::Value& value,
    hdf5::Issues& issues)
{
    // - For each location in time, a value array is stored in the
    //   value. The number of these values arrays must be equal to the
    //   number of active sets stored in the object tracker.
    // - The size of each value array must equal the size of each active set.

    auto const& active_set_index = object_tracker.active_set_index();

    auto const nr_active_sets = active_set_index.nr_indices();
    auto const nr_value_arrays = value.nr_locations_in_time();

    if(nr_value_arrays != nr_active_sets) {
        issues.add_error(value.id(), fmt::format(
            "Number of value arrays in value does not equal "
            "the number of active sets in object tracker "
            "({} != {})",
            nr_value_arrays, nr_active_sets
        ));
    }
    else {

        // Iterate over each active set and verify that the size of the
        // value array equals the size of the set
        std::vector<Index> set_idxs(active_set_index.nr_indices());
        active_set_index.read(set_idxs.data());

        // Add an end index to ease the iteration over ranges of IDs
        set_idxs.push_back(object_tracker.active_id().nr_ids());
        auto begin_idx = set_idxs[0];

        for(std::size_t i = 1; i < set_idxs.size(); ++i) {
            auto const end_idx = set_idxs[i];
            auto const active_set_size = end_idx - begin_idx;

            auto const value_array = value[i - 1];
            auto const nr_object_arrays = value_array.nr_arrays();

            if(nr_object_arrays != active_set_size) {
                issues.add_error(value.id(), fmt::format(
                    "Number of object arrays stored does not equal "
                    "the size of the active set "
                    "({} != {})",
                    nr_object_arrays, active_set_size
                ));

                break;
            }

            begin_idx = end_idx;
        }
    }
}


void validate(
    ObjectTracker const& /* object_tracker */,
    different_shape::variable_shape::Value const& /* value */,
    hdf5::Issues& /* issues */)
{
    throw_not_supported_yet(
        "validation of different_shape::variable_shape::Value");
}


void validate(
    info::ID const& id,
    hdf5::Issues& issues)
{
    std::vector<ID> ids(id.nr_objects());

    id.read(ids.data());

    assert_ids_are_unique(id.id(), std::move(ids), issues);
}


void validate(
    info::ActiveSetIndex const& active_set_index,
    info::ActiveID const& active_id,
    hdf5::Issues& issues)
{
    std::vector<Index> set_idxs(active_set_index.nr_indices());
    std::vector<ID> object_ids(active_id.nr_ids());

    active_set_index.read(set_idxs.data());
    active_id.read(object_ids.data());

    if(set_idxs.empty()) {
        if(!object_ids.empty()) {
            issues.add_error(active_set_index.id(), fmt::format(
                "{} IDs of active objects are stored, "
                "but indices of active sets are missing",
                object_ids.size()
            ));
        }
    }
    else {
        if(object_ids.empty()) {
            issues.add_error(active_set_index.id(), fmt::format(
                "{} indices of active sets are stored, "
                "but IDs of active objects are missing",
                set_idxs.size()
            ));
        }

        // Validate indices of active sets and IDs of active objects
        // Add an end index to ease the iteration over ranges of IDs
        set_idxs.push_back(object_ids.size());
        auto begin_idx = set_idxs[0];

        for(std::size_t i = 1; i < set_idxs.size(); ++i) {
            auto const end_idx = set_idxs[i];

            // Validate active set indices are valid
            if(end_idx < begin_idx) {
                issues.add_error(active_set_index.id(), fmt::format(
                    "Start indices of active sets must be "
                    "monotonic increasing, but "
                    "next index {} < current index {}",
                    end_idx, begin_idx
                ));

                break;
            }

            // Validate active set is stored
            if(end_idx > object_ids.size()) {
                issues.add_error(active_set_index.id(), fmt::format(
                    "Part of the collection of active object IDs is "
                    "missing (end index {} > collection size {})",
                    end_idx, object_ids.size()
                ));

                break;
            }

            // Validate active object IDs are valid
            if(!assert_ids_are_unique(
                    active_id.id(),
                    std::vector<ID>(
                        object_ids.begin() + begin_idx, object_ids.begin() + end_idx),
                    issues)) {
                break;
            }

            begin_idx = end_idx;
        }
    }
}


void validate(
    info::ActiveSetIndex const& active_set_index,
    info::ActiveObjectIndex const& active_object_index,
    hdf5::Issues& issues)
{
    std::vector<Index> set_idxs(active_set_index.nr_indices());
    std::vector<Index> object_idxs(active_object_index.nr_indices());

    active_set_index.read(set_idxs.data());
    active_object_index.read(object_idxs.data());

    if(set_idxs.empty()) {
        if(!object_idxs.empty()) {
            issues.add_error(active_set_index.id(), fmt::format(
                "{} Indices of active objects are stored, "
                "but indices of active sets are missing",
                object_idxs.size()
            ));
        }
    }
    else if(!object_idxs.empty()) {
        // Validate indices of active sets and indices of active objects
        // Add an end index to ease the iteration over ranges of indices
        set_idxs.push_back(object_idxs.size());
        auto begin_idx = set_idxs[0];

        for(std::size_t i = 1; i < set_idxs.size(); ++i) {
            auto const end_idx = set_idxs[i];

            // Validate active set indices are valid
            if(end_idx < begin_idx) {
                issues.add_error(active_set_index.id(), fmt::format(
                    "Start indices of active sets must be "
                    "monotonic increasing, but "
                    "next index {} < current index {}",
                    end_idx, begin_idx
                ));

                break;
            }

            // Validate active set is stored
            if(end_idx > object_idxs.size()) {
                issues.add_error(active_set_index.id(), fmt::format(
                    "Part of the collection of active object indices is "
                    "missing (end index {} > collection size {})",
                    end_idx, object_idxs.size()
                ));

                break;
            }

            begin_idx = end_idx;
        }
    }
}


void validate(
    ObjectTracker const& object_tracker,
    hdf5::Issues& issues)
{
    validate(object_tracker.id(), issues);
    validate(
        object_tracker.active_set_index(),
        object_tracker.active_id(),
        issues);
    validate(
        object_tracker.active_set_index(),
        object_tracker.active_object_index(),
        issues);

    // TODO Test whether the combination of tracking stuff makes sense
    // - size of active sets (index or id) cannot exceed size of id collection
    //     (but id collection can be empty in that case)
    // auto const& id = object_tracker.id();
}


void validate(
    ObjectTracker const& object_tracker,
    TimeBox const& time_box,
    hdf5::Issues& issues)
{
    {
        std::vector<time::DurationCount> boxes(2 * time_box.nr_boxes());
        time_box.read(boxes.data());

        assert_strictly_increasing(time_box.id(), boxes, issues);
    }

    // - Per location in time, the object tracker contains information
    //   about which objects are active. The number of these active sets
    //   must equal the number of object arrays.
    {
        auto const nr_active_sets =
            object_tracker.active_set_index().nr_indices();
        auto const nr_boxes = time_box.nr_boxes();

        if(nr_boxes != nr_active_sets) {
            issues.add_error(time_box.id(), fmt::format(
                "Number of boxes in time domain does not equal "
                "the number of active set indices in object tracker "
                "({} != {})",
                nr_boxes, nr_active_sets
            ));
        }
    }
}


void validate(
    ObjectTracker const& object_tracker,
    TimePoint const& time_point,
    hdf5::Issues& issues)
{
    {
        std::vector<time::DurationCount> points(2 * time_point.nr_points());
        time_point.read(points.data());

        assert_strictly_increasing(time_point.id(), points, issues);
    }

    // - Per location in time, the object tracker contains information
    //   about which objects are active. The number of these active sets
    //   must equal the number of object arrays.
    {
        auto const nr_active_sets =
            object_tracker.active_set_index().nr_indices();
        auto const nr_points = time_point.nr_points();

        if(nr_points != nr_active_sets) {
            issues.add_error(time_point.id(), fmt::format(
                "Number of points in time domain does not equal "
                "the number of active set indices in object tracker "
                "({} != {})",
                nr_points, nr_active_sets
            ));
        }
    }
}


void validate(
    ObjectTracker const& object_tracker,
    TimeDomain& time_domain,
    hdf5::Issues& issues)
{
    auto const configuration = time_domain.configuration();

    switch(configuration.value<TimeDomainItemType>()) {
        case TimeDomainItemType::box: {
            validate(object_tracker, time_domain.value<TimeBox>(), issues);
            break;
        }
        case TimeDomainItemType::point: {
            validate(object_tracker, time_domain.value<TimePoint>(), issues);
            break;
        }
    }
}


void validate(
    ObjectTracker const& object_tracker,
    StationarySpaceBox const& space_box,
    hdf5::Issues& issues)
{
    // A stationary space box is a same_shape::Value
    using Value = same_shape::Value;

    validate(
        object_tracker, dynamic_cast<Value const&>(space_box), issues);
}


void validate(
    ObjectTracker const& object_tracker,
    StationarySpacePoint const& space_point,
    hdf5::Issues& issues)
{
    // A stationary space point is a same_shape::Value
    using Value = same_shape::Value;

    validate(
        object_tracker, dynamic_cast<Value const&>(space_point), issues);
}


void validate(
    ObjectTracker const& object_tracker,
    MobileSpaceBox const& space_box,
    hdf5::Issues& issues)
{
    // A mobile space box is a same_shape::constant_shape::Value
    using Value = same_shape::constant_shape::Value;

    validate(
        object_tracker, dynamic_cast<Value const&>(space_box), issues);
}


void validate(
    ObjectTracker const& object_tracker,
    MobileSpacePoint const& space_point,
    hdf5::Issues& issues)
{
    // A mobile space point is a same_shape::constant_shape::Value
    using Value = same_shape::constant_shape::Value;

    validate(
        object_tracker, dynamic_cast<Value const&>(space_point), issues);
}


void validate(
    ObjectTracker const& object_tracker,
    SpaceDomain& space_domain,
    hdf5::Issues& issues)
{
    auto const configuration = space_domain.configuration();
    auto const mobility = configuration.value<Mobility>();
    auto const item_type = configuration.value<SpaceDomainItemType>();

    switch(mobility) {
        case Mobility::mobile: {
            switch(item_type) {
                case SpaceDomainItemType::box: {
                    validate(
                        object_tracker,
                        space_domain.value<MobileSpaceBox>(), issues);
                    break;
                }
                case SpaceDomainItemType::point: {
                    validate(
                        object_tracker,
                        space_domain.value<MobileSpacePoint>(), issues);
                    break;
                }
            }

            break;
        }
        case Mobility::stationary: {
            switch(item_type) {
                case SpaceDomainItemType::box: {
                    validate(
                        object_tracker,
                        space_domain.value<StationarySpaceBox>(), issues);
                    break;
                }
                case SpaceDomainItemType::point: {
                    validate(
                        object_tracker,
                        space_domain.value<StationarySpacePoint>(), issues);
                    break;
                }
            }

            break;
        }
    }
}


void validate(
    ObjectTracker const& object_tracker,
    same_shape::Property& property,
    hdf5::Issues& issues)
{
    validate(object_tracker, property.value(), issues);

    if(property.time_is_discretized()) {
        throw_not_supported_yet(
            "validation of discretization through time of "
            "same_shape::Property");
    }

    if(property.space_is_discretized()) {
        throw_not_supported_yet(
            "validation of discretization through space of "
            "same_shape::Property");
    }
}


void validate(
    ObjectTracker const& object_tracker,
    different_shape::Property& property,
    hdf5::Issues& issues)
{
    auto const& property_value = property.value();

    validate(object_tracker, property_value, issues);

    if(property.time_is_discretized()) {
        throw_not_supported_yet(
            "validation of discretization through time of "
            "different_shape::Property");
    }

    if(property.space_is_discretized()) {
        // Each property value is discretized through space
        auto discretization_property = property.space_discretization_property();
        auto const& discretization_property_name =
            discretization_property.name();
        PropertySet discretization_property_set{
            discretization_property.property_set_group()};
        auto const& properties = discretization_property_set.properties();
        bool values_can_be_tested = false;


        if(properties.value_variability(discretization_property_name) !=
                ValueVariability::constant) {
            issues.add_error(discretization_property.id(),
                "Property values for discretization of constant "
                "property values must be constant themselves");
            values_can_be_tested = false;
        }

        switch(property.space_discretization_type()) {
            case SpaceDiscretization::regular_grid: {
                // For each dimension, the discretization property
                // value contains a count
                // - The discretization property must
                //      - Be of type same_shape::Property
                //      - Contain counts (integral values?)
                // - These counts must match the value's shape.
                // - The number of values in the discretization property
                //   must match the number of values discretized.
                if(properties.shape_per_object(discretization_property_name) !=
                        ShapePerObject::same) {
                    issues.add_error(property.id(),
                        "Property values of regular grid must be "
                        "the same for each object");
                    values_can_be_tested = false;
                }

                if(values_can_be_tested) {
                    auto const& discretization_value =
                        properties.collection<same_shape::Properties>()[
                            discretization_property_name].value();

                    if(discretization_value.nr_arrays() !=
                            property_value.nr_objects()) {
                        issues.add_error(
                                discretization_property.id(),
                                fmt::format(
                            "Number of values in discretization property "
                            "must equal the number of values discretized "
                            "({} != {})",
                            discretization_value.nr_arrays(),
                            property_value.nr_objects()));
                    }

                    if(!hdf5::is_native_unsigned_integral(
                            discretization_value.memory_datatype())) {
                        issues.add_error(
                                discretization_property.id(),
                                fmt::format(
                            "Discretization property must contain unsigned "
                            "integral values "
                            "({} is not unsigned integral)",
                            hdf5::native_datatype_as_string(
                                discretization_value.memory_datatype())));
                    }

                    if(discretization_value.array_shape() !=
                            hdf5::Shape{
                                static_cast<Count>(property_value.rank())}) {
                        issues.add_error(
                                discretization_property.id(),
                            "For each spatial dimension, the discretization "
                            "property must contain a count");
                    }

                    // TODO Compare the counts. These must match the shapes
                    //      of the values
                }

                break;
            }
        }
    }
}


void validate(
    ObjectTracker const& object_tracker,
    same_shape::constant_shape::Property& property,
    hdf5::Issues& issues)
{
    validate(object_tracker, property.value(), issues);

    if(property.time_is_discretized()) {
        throw_not_supported_yet(
            "validation of discretization through time of "
            "same_shape::constant_shape::Property");
    }

    if(property.space_is_discretized()) {
        throw_not_supported_yet(
            "validation of discretization through space of "
            "same_shape::constant_shape::Property");
    }
}


void validate(
    ObjectTracker const& object_tracker,
    different_shape::constant_shape::Property& property,
    hdf5::Issues& issues)
{
    validate(object_tracker, property.value(), issues);

    if(property.time_is_discretized()) {
        throw_not_supported_yet(
            "validation of discretization through time of "
            "different_shape::constant_shape::Property");
    }

    if(property.space_is_discretized()) {
        throw_not_supported_yet(
            "validation of discretization through space of "
            "different_shape::constant_shape::Property");
    }
}


void validate(
    ObjectTracker const& object_tracker,
    same_shape::variable_shape::Property& property,
    hdf5::Issues& issues)
{
    auto& property_value = property.value();

    validate(object_tracker, property_value, issues);

    if(property.time_is_discretized()) {

        // Each property value is discretized through time
        auto discretization_property = property.time_discretization_property();
        auto const& discretization_property_name =
            discretization_property.name();
        PropertySet discretization_property_set{
            discretization_property.property_set_group()};
        auto const& properties = discretization_property_set.properties();
        bool values_can_be_tested = false;


        if(properties.value_variability(discretization_property_name) !=
                ValueVariability::variable) {
            issues.add_error(discretization_property.id(),
                "Property values for discretization of variable "
                "property values must be variable themselves");
            values_can_be_tested = false;
        }

        switch(property.time_discretization_type()) {
            case TimeDiscretization::regular_grid: {
                // Discretization property must be a collection property
                // (nr objects == 1). For each location in time,
                // the discretization property contains a count
                // (same_shape::variable_shape::Value, unsigned integral).
                // The number of counts in the discretization property
                // must match the number of locations in time in the
                // discretized property.

                if(properties.shape_per_object(discretization_property_name) !=
                        ShapePerObject::same) {
                    issues.add_error(property.id(),
                        "Property values of regular grid must be "
                        "the same for each object");
                    values_can_be_tested = false;
                }

                if(properties.shape_variability(discretization_property_name) !=
                        ShapeVariability::constant) {
                    issues.add_error(property.id(),
                        "Shape of property values of regular grid must be "
                        "constant through time");
                    values_can_be_tested = false;
                }

                if(values_can_be_tested) {
                    auto const& discretization_value =
                        properties.collection<
                            same_shape::constant_shape::Properties>()[
                                discretization_property_name].value();

                    if(discretization_value.nr_arrays() !=
                            property_value.nr_locations_in_time()) {
                        issues.add_error(
                                discretization_property.id(),
                                fmt::format(
                            "Number of counts in discretization property "
                            "must equal the number of locations in time "
                            "for which values are discretized "
                            "({} != {})",
                            discretization_value.nr_arrays(),
                            property_value.nr_locations_in_time()));
                    }

                    if(!hdf5::is_native_unsigned_integral(
                            discretization_value.memory_datatype())) {
                        issues.add_error(
                                discretization_property.id(),
                                fmt::format(
                            "Discretization property must contain unsigned "
                            "integral values "
                            "({} is not unsigned integral)",
                            hdf5::native_datatype_as_string(
                                discretization_value.memory_datatype())));
                    }

                    if(discretization_value.array_shape().size() != 1) {
                        issues.add_error(
                            discretization_property.id(), fmt::format(
                                "The rank of the discretization property "
                                "values must be 1, but it currently is {}",
                                discretization_value.array_shape().size()));
                    }
                    else if(discretization_value.array_shape()[0] !=
                            property_value.nr_locations_in_time()) {
                        issues.add_error(
                            discretization_property.id(), fmt::format(
                                "The number of counts in the discretization "
                                "property must equal the number of locations "
                                "in time in the discretized property "
                                "({} != {})",
                                discretization_value.array_shape()[0],
                                property_value.nr_locations_in_time()));

                    }
                }

                break;
            }
        }
    }

    if(property.space_is_discretized()) {
        throw_not_supported_yet(
            "validation of discretization through space of "
            "same_shape::variable_shape::Property");
    }
}


void validate(
    ObjectTracker const& object_tracker,
    different_shape::variable_shape::Property& property,
    hdf5::Issues& issues)
{
    validate(object_tracker, property.value(), issues);

    if(property.time_is_discretized()) {
        throw_not_supported_yet(
            "validation of discretization through time of "
            "different_shape::variable_shape::Property");
    }

    if(property.space_is_discretized()) {
        throw_not_supported_yet(
            "validation of discretization through space of "
            "different_shape::variable_shape::Property");
    }
}


void validate(
    ObjectTracker const& object_tracker,
    Properties& properties,
    hdf5::Issues& issues)
{
    for(auto& property:
            properties.collection<same_shape::Properties>()) {
        validate(object_tracker, property.second, issues);
    }

    for(auto& property: properties.collection<
            different_shape::Properties>()) {
        validate(object_tracker, property.second, issues);
    }

    for(auto& property: properties.collection<
            same_shape::constant_shape::Properties>()) {
        validate(object_tracker, property.second, issues);
    }

    for(auto& property: properties.collection<
            different_shape::constant_shape::Properties>()) {
        validate(object_tracker, property.second, issues);
    }

    for(auto& property: properties.collection<
            same_shape::variable_shape::Properties>()) {
        validate(object_tracker, property.second, issues);
    }

    for(auto& property: properties.collection<
            different_shape::variable_shape::Properties>()) {
        validate(object_tracker, property.second, issues);
    }
}


void validate(
    PropertySet& property_set,
    hdf5::Issues& issues)
{
    auto const& object_tracker = property_set.object_tracker();
    auto nr_errors = issues.errors().size();
    validate(object_tracker, issues);
    bool const object_tracker_is_valid = issues.errors().size() == nr_errors;

    if(object_tracker_is_valid) {

        if(property_set.has_time_domain()) {
            validate(object_tracker, property_set.time_domain(), issues);
        }

        if(property_set.has_space_domain()) {
            validate(object_tracker, property_set.space_domain(), issues);
        }

        validate(object_tracker, property_set.properties(), issues);
    }
}


void validate(
    Phenomenon& phenomenon,
    hdf5::Issues& issues)
{
    auto& property_sets = phenomenon.property_sets();

    for(auto const& name: property_sets.names()) {
        validate(property_sets[name], issues);
    }


    auto& collection_property_sets = phenomenon.collection_property_sets();

    for(auto const& name: collection_property_sets.names()) {
        auto& property_set = collection_property_sets[name];
        auto& properties = property_set.properties();

        validate(property_set, issues);


        // Assert size of collection in each collection property set is 1
        auto const& object_tracker = property_set.object_tracker();

        {
            std::vector<bool> check_ids;

            {
                using Properties = same_shape::Properties;
                auto const& collection = properties.collection<Properties>();
                check_ids.push_back(!collection.empty());
            }

            {
                using Properties = different_shape::Properties;
                auto const& collection = properties.collection<Properties>();
                check_ids.push_back(!collection.empty());
            }

            if(std::any_of(check_ids.begin(), check_ids.end(),
                    [](bool const b) { return b; })) {

                auto const nr_objects = object_tracker.id().nr_objects();

                if(nr_objects != 1) {
                    issues.add_error(object_tracker.id().id(), fmt::format(
                        "Number of object IDs in object tracker "
                        "of collection properties does not equal 1 "
                        "({} != 1)",
                        nr_objects
                    ));

                }
            }
        }

        {
            std::vector<bool> check_active_sets;
            bool check_active_object_indices = false;

            {
                using Properties = same_shape::constant_shape::Properties;
                auto const& collection = properties.collection<Properties>();
                check_active_sets.push_back(!collection.empty());
            }

            {
                using Properties = same_shape::variable_shape::Properties;
                auto const& collection = properties.collection<Properties>();
                check_active_sets.push_back(!collection.empty());
            }

            {
                using Properties = different_shape::constant_shape::Properties;
                auto const& collection = properties.collection<Properties>();
                check_active_sets.push_back(!collection.empty());
                check_active_object_indices = !collection.empty();
            }

            if(std::any_of(check_active_sets.begin(), check_active_sets.end(),
                    [](bool const b) { return b; })) {

                // In case the number of objects is one, as it should, than
                // the number of active sets equals the number of ids
                auto const nr_active_sets =
                    object_tracker.active_set_index().nr_arrays();
                auto const nr_objects =
                    object_tracker.active_id().nr_ids();

                if(nr_active_sets != nr_objects) {
                    issues.add_error(object_tracker.id().id(), fmt::format(
                        "Size of each active set in object tracker "
                        "of collection properties does not equal "
                        "the number of active object IDs "
                        "({} != {})",
                        nr_active_sets, nr_objects
                    ));
                }

                if(check_active_object_indices) {
                    auto const nr_objects =
                        object_tracker.active_id().nr_ids();

                    if(nr_active_sets != nr_objects) {
                        issues.add_error(object_tracker.id().id(), fmt::format(
                            "Size of each active set in object tracker "
                            "of collection properties does not equal "
                            "the number of active object indices "
                            "({} != {})",
                            nr_active_sets, nr_objects
                        ));
                    }
                }
            }
        }
    }
}


void validate(
    Universe& universe,
    hdf5::Issues& issues)
{
    auto& phenomena = universe.phenomena();

    for(auto const& name: phenomena.names()) {
        validate(phenomena[name], issues);
    }
}


void validate(
    Dataset& dataset,
    hdf5::Issues& issues)
{
    auto& universes = dataset.universes();

    for(auto const& name: universes.names()) {
        validate(universes[name], issues);
    }


    auto& phenomena = dataset.phenomena();

    for(auto const& name: phenomena.names()) {
        validate(phenomena[name], issues);
    }
}


/*!
    @brief      Validate whether @a file contains a LUE dataset and report
                any @a issues
*/
void validate(
    hdf5::File const& file,
    hdf5::Issues& issues)
{
    // TODO
    // - test version attribute (error)
    // - test history attribute (warning)

    // TODO
    //     history attribute on all levels, maybe with only information
    //     about the contained information?
    // std::vector<std::string> expected_attribute_names{
    //     history_tag,
    //     version_tag,
    // };
    // assert_attributes_exist(file, expected_attribute_names, issues);


    // std::vector<std::string> expected_group_names{
    //     phenomena_tag,
    //     universes_tag,
    // };
    // auto const groups_exist = hdf5::assert_groups_exist(
    //     file, expected_group_names, issues);

    // if(groups_exist) {
        // Catching exceptions, because we start using the lue
        // API. Exceptions may be thrown when expected HDF5 objects are
        // not found.
        try {
            Dataset dataset{file.pathname()};
            validate(dataset, issues);
        }
        catch(std::exception const& exception) {
            issues.add_error(file.id(), exception.what());
        }

    // }



    // std::vector<std::string> expected_object_names = expected_group_names;
    // hdf5::assert_no_superfluous_objects_exist(
    //     file, expected_object_names, issues);






    // // if(file.contains_group("lue_universes")) {
    // //     validate_universes(hdf5::open_group(file, "lue_universes"), issues);
    // // }


    // // if(file.contains_group("lue_phenomena")) {
    // //     validate_phenomena(hdf5::open_group(file, "lue_phenomena"), issues);
    // // }




    // // std::vector<std::string> expected_object_names = expected_group_names;
    // // assert_no_superfluous_objects_exist(file, expected_object_names, issues);


    // // auto const& universes = dataset.universes();

    // // for(auto const& name: universes.names()) {
    // //     validate(universes[name], issues);
    // // }

    // // auto const& phenomena = dataset.phenomena();

    // // for(auto const& name: phenomena.names()) {
    // //     validate(phenomena[name], issues);
    // // }
}


/*!
    @brief      Validate file named @a pathname
    @param      issues Collection of issues found
*/
void validate(
    std::string const& pathname,
    hdf5::Issues& issues)
{
    validate(hdf5::File(pathname), issues);
}


/*!
    @brief      Assert that the dataset in @a file is valid
    @exception  std::runtime_error In case the dataset is not valid
*/
void assert_is_valid(
    hdf5::File const& file,
    bool const fail_on_warning)
{
    hdf5::Issues issues;

    validate(file, issues);

    if(issues.errors_found() || (fail_on_warning && issues.warnings_found())) {
        throw std::runtime_error(error_message(issues));
    }
}


/*!
    @brief      Assert that the dataset in @a dataset_name is valid
    @exception  std::runtime_error In case the dataset is not valid
*/
void assert_is_valid(
    std::string const& dataset_name,
    bool const fail_on_warning)
{
    assert_is_valid(hdf5::File(dataset_name), fail_on_warning);
}

}  // namespace lue
