add_hpx_executable(lue_airpolution_benchmark
    SOURCES
        airpolution_benchmark
)

target_link_libraries(lue_airpolution_benchmark
    PRIVATE
        lue::airpolution
        lue::benchmark
)
