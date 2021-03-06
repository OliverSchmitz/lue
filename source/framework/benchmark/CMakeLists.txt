add_library(lue_benchmark
    src/environment
    src/runtime_environment
    src/stopwatch
    src/task
    src/time_interval
)

add_library(lue::benchmark ALIAS lue_benchmark)

target_include_directories(lue_benchmark
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

hpx_setup_target(lue_benchmark
    DEPENDENCIES
        lue::framework_core
        docopt
        nlohmann_json::nlohmann_json
)

add_executable(lue_dummy_benchmark
    src/dummy_benchmark
)

target_link_libraries(lue_dummy_benchmark
    PRIVATE
        lue::benchmark
        # nlohmann_json::nlohmann_json
)

add_hpx_executable(lue_dummy_hpx_benchmark
    SOURCES
        src/dummy_hpx_benchmark
)

target_link_libraries(lue_dummy_hpx_benchmark
    PRIVATE
        lue::benchmark
)
