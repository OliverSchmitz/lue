import os.path


### def output_pathname(
###         job_script_pathname):
### 
###     return "{}.out".format(job_script_pathname)

###     # template = "{}-{{}}.out".format(job_script_pathname)
### 
###     # i = 0
###     # result = template.format(i)
### 
###     # while os.path.exists(result):
###     #     i += 1
###     #     result = template.format(i)
### 
###     # return result


def srun_configuration(
        cluster):
    """
    Return common arguments to the srun command used to start the program
    being benchmarked
    """
    assert cluster.scheduler.kind == "slurm"
    return " ".join(cluster.scheduler.settings.srun_options)


def program_configuration(
        cluster,
        benchmark,
        experiment,
        array_shape,
        partition_shape,
        result_pathname=None,
        nr_workers=None):

    assert nr_workers is not None

    if result_pathname is None:
        assert not nr_workers is None
        result_pathname = experiment.benchmark_result_pathname(
            cluster.name, benchmark.scenario_name, nr_workers, "json")

    #   '--hpx:queuing=shared-priority '
    #   '--hpx:bind=balanced '
    #   '--hpx:numa-sensitive '
    #   '--hpx:use-process-mask '
    #   '--hpx:bind="{thread_binding}" '
    configuration = \
        '--hpx:print-bind ' \
        '--hpx:ini="application.{program_name}.benchmark.cluster_name!={cluster_name}" ' \
        '--hpx:ini="application.{program_name}.benchmark.count!={count}" ' \
        '--hpx:ini="application.{program_name}.benchmark.nr_workers!={nr_workers}" ' \
        '--hpx:ini="application.{program_name}.benchmark.output!={result_pathname}" ' \
        '--hpx:ini="application.{program_name}.nr_time_steps!={nr_time_steps}" ' \
        '--hpx:ini="application.{program_name}.array_shape!={array_shape}" ' \
        '--hpx:ini="application.{program_name}.partition_shape!={partition_shape}" ' \
            .format(
                # thread_binding=benchmark.thread_binding,
                program_name=experiment.program_name,
                cluster_name=cluster.name,
                count=benchmark.count,
                nr_workers=nr_workers,
                nr_time_steps=experiment.nr_time_steps,
                array_shape=list(array_shape),
                partition_shape=list(partition_shape),
                result_pathname=result_pathname,
            )

    arguments = []

    if experiment.max_tree_depth is not None:
        arguments.append(
            '--hpx:ini="application.{program_name}.benchmark.max_tree_depth!={max_tree_depth}"'
                .format(
                    program_name=experiment.program_name,
                    max_tree_depth=experiment.max_tree_depth))

    if not benchmark.hpx is None:

        performance_counters = benchmark.hpx.performance_counters

        if performance_counters is not None:

            assert not nr_workers is None
            counter_pathname = experiment.benchmark_result_pathname(
                cluster.name, benchmark.scenario_name,
                "counter-{}".format(nr_workers), "csv")

            # Format arguments for tracking performance counters
            arguments += [
                '--hpx:print-counter-format=csv',
                '--hpx:print-counter-destination="{destination}"'.format(
                    destination=counter_pathname),
            ]

            for argument in performance_counters:
                # Each argument is a dict with a single item. Each dict
                # maps a key (the argument name) to a value. Value can
                # be a single value or a list of values.
                assert len(argument) == 1, argument
                items = list(argument.items())
                assert len(items) == 1
                assert len(items[0]) == 2
                # assert len(argument.items()[0]) == 2, argument.items()
                key, value = items[0]

                if not isinstance(value, list):
                    arguments.append('--hpx:{}="{}"'.format(key, value))
                else:
                    for item in value:
                        arguments.append('--hpx:{}="{}"'.format(key, item))

    configuration += " {}".format(" ".join(arguments))

    return configuration


def write_script(
        commands,
        script_pathname):

    with open(script_pathname, "w") as script:
        script.write("""\
#!/usr/bin/env bash
set -e

{commands}
""".format(
            commands="\n".join(commands)
        ))


def create_slurm_script(
        nr_cluster_nodes,  # How many nodes to reserve
        nr_tasks,  # How many tasks to reserve for
        nr_cores_per_socket,  # Number of physical cores per socket
        # nr_cores_per_numa_node,
        # nr_threads,  # Total nr of threads, including HT threads
        cpus_per_task,
        output_filename,
        partition_name,
        max_duration,
        job_steps):

    # HPX doc:
    # You can change the number of localities started per node
    # (for example to account for NUMA effects) by specifying the
    # -n option of srun. The number of cores per locality can be
    # set by -c.

    # There is no need to use any of the HPX command line options
    # related to the number of localities, number of threads,
    # or related to networking ports. All of this information is
    # automatically extracted from the SLURM environment by the
    # HPX startup code.

    # The srun documentation explicitly states: "If -c is
    # specified without -n as many tasks will be allocated per node
    # as possible while satisfying the -c restriction. For instance
    # on a cluster with 8 CPUs per node, a job request for 4 nodes
    # and 3 CPUs per task may be allocated 3 or 6 CPUs per node (1
    # or 2 tasks per node) depending upon resource consumption by
    # other jobs." For this reason, we suggest to always specify
    # -n <number-of-instances>, even if <number-of-instances>
    # is equal to one (1).

    # In SLURM, a CPU is either a core or a thread, depending on the
    # system configuration
    #SBATCH --sockets-per-node=2

    #SBATCH --threads-per-core=1
    ### #SBATCH --cores-per-socket={cores_per_socket}

    return """\
#!/usr/bin/env bash
#SBATCH --nodes={nr_cluster_nodes}
#SBATCH --ntasks={nr_tasks}
#SBATCH --cpus-per-task={cpus_per_task}
#SBATCH --output={output_filename}
#SBATCH --cores-per-socket={cores_per_socket}
#SBATCH --partition={partition_name}
{max_duration}

set -e

module purge
module load opt/all
module load userspace/all
module load gcc/7.2.0
module load boost/gcc72/1.65.1
module load mpich/gcc72/mlnx/3.2.1
module load libraries/papi/5.7.0

{job_steps}""".format(
        nr_cluster_nodes=nr_cluster_nodes,  # Ask for this nr_cluster_nodes
        nr_tasks=nr_tasks,  # Ask for hardware for max nr_tasks
        cores_per_socket=nr_cores_per_socket,  # Ask for whole sockets of core
        # cores_per_socket=nr_cores_per_numa_node,
        cpus_per_task=cpus_per_task,
        output_filename=output_filename,
        partition_name=partition_name,
        max_duration="#SBATCH --time={}".format(max_duration)
            if max_duration is not None else "",
        job_steps="\n".join(job_steps))
