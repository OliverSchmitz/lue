add_unit_tests(
    SCOPE lue_framework_core
    NAMES
        array_partition_data_test
        array_partition_definition_test
        array_partition_test
        domain_decomposition_test
        partitioned_array_test
        shape_test
    LINK_LIBRARIES
        lue::framework_core
        lue::framework_test
)

# Targets that depend on HPX
hpx_setup_target(lue_framework_core_array_partition_test)
hpx_setup_target(lue_framework_core_partitioned_array_test)

# set_target_properties(lue_framework_core_array_partition_test PROPERTIES COMPILE_FLAGS "-E")
# set_target_properties(lue_framework_core_partitioned_array_test PROPERTIES COMPILE_FLAGS "-E")