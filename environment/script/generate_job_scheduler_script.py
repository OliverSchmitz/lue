#!/usr/bin/env python
import os.path
import sys
sys.path = [
    os.path.join(os.path.split(__file__)[0], "..", "..", "benchmark", "lue")
] + sys.path
import benchmark
import docopt


usage = """\
Generate job scheduler script for benchmarking

Usage:
    {command} (partition_shape|strong_scaling|weak_scaling)
        <cluster_settings> <benchmark_settings> <experiment_settings>
        <script> <command>

Options:
    partition_shape   Benchmark for partition shape
    strong_scaling    Benchmark for strong scaling characteristics
    weak_scaling      Benchmark for weak scaling characteristics
    cluster_settings  Pathname of JSON file containing cluster settings
    experiment_settings  Pathname of JSON file containing experiment settings
    script            Pathname of shell script to generate
    command           Command to benchmark
    -h --help         Show this screen

The generated shell script will submit all required tasks to the scheduler

Experiment settings:

    partition_shape
        ...

    strong_scaling
        ...

    weak_scaling
        ...
""".format(
    command = os.path.basename(sys.argv[0]))


if __name__ == "__main__":
    arguments = docopt.docopt(usage)

    cluster_settings_pathname = arguments["<cluster_settings>"]
    cluster_settings = benchmark.json_to_data(cluster_settings_pathname)

    benchmark_settings_pathname = arguments["<benchmark_settings>"]
    benchmark_settings = benchmark.json_to_data(benchmark_settings_pathname)

    experiment_settings_pathname = arguments["<experiment_settings>"]
    experiment_settings = benchmark.json_to_data(experiment_settings_pathname)

    script_pathname = arguments["<script>"]
    command_pathname = arguments["<command>"]

    if arguments["partition_shape"]:
        benchmark.partition_shape.generate_script(
            cluster_settings, benchmark_settings, experiment_settings,
            script_pathname, command_pathname)
    elif arguments["strong_scaling"]:
        benchmark.strong_scaling.generate_script(
            cluster_settings, benchmark_settings, experiment_settings,
            script_pathname, command_pathname)
    elif arguments["weak_scaling"]:
        benchmark.weak_scaling.generate_script(
            cluster_settings, benchmark_settings, experiment_settings,
            script_pathname, command_pathname)
