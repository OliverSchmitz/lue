#pragma once
#include "lue/framework/benchmark/environment.hpp"
#include "lue/framework/benchmark/stopwatch.hpp"
#include "lue/framework/benchmark/timing.hpp"
#include <iostream>
#include <string>
#include <vector>


namespace lue {
namespace benchmark {

/*
Notes

Benchmark:
- Measure performance of some solution (exe, parameters, system)
- Benchmark is an exe
    - Work to benchmark must be a callable in a lib
    - Each run must be a call to this function, timed by the benchmark
        instance
- Description
- Meta-information
    - exe
    - parameters
    - system
    - ...
- Timings (multiple equivalent runs, capture spread)

Benchmark
    description
    system
    exe
        path
        build_configuration
    parameters
    timings
        timing
            start
            end

HPXBenchmark
    system
        localities
        ...
    hpx
        build_configuration
*/


/*!
    @brief      Class for managing a performance benchmark

    A performance benchmark is an experiment to see how fast a piece of
    code executeѕ. Benchmark instances will manage the timing of a
    single piece of code. Multiple timings can be collected in order to
    obtain information about the spread in the results.
*/
template<
    typename Callable>
class Benchmark
{

public:

    using Duration = Stopwatch::Duration;
    using Timings = std::vector<Timing>;

                   Benchmark           (Callable&& callable,
                                        std::string const& system_name,
                                        Environment const& environment,
                                        std::string const& name,
                                        std::string const& description);

                   Benchmark           (Benchmark const&)=delete;

                   Benchmark           (Benchmark&&)=delete;

                   ~Benchmark          ()=default;

    Benchmark&     operator=           (Benchmark const&)=delete;

    Benchmark&     operator=           (Benchmark&&)=delete;

    std::string const& system_name     () const;

    std::string const& name            () const;

    std::string const& description     () const;

    Environment const& environment     () const;

    int            run                 ();

    TimeInterval const& time_interval  () const;

    Timings const& timings             () const;

private:

    //! Callable representing the workload to time
    Callable       _callable;

    //! Name of system the benchmark runs on: hostname or cluster name
    std::string const _system_name;

    //! Environment the benchmark runs in
    Environment const _environment;

    //! Name of the benchmark
    std::string const _name;

    //! Description of the benchmark
    std::string const _description;

    //! Interval in which the benchmark ran
    TimeInterval   _time_interval;

    //! Timings of the @a count benchmark runs
    Timings        _timings;

};


/*!
    @brief      Construct an instance

    In case of weird errors, verify that the callable is moved in.
*/
template<
    typename Callable>
inline Benchmark<Callable>::Benchmark(
    Callable&& callable,
    std::string const& system_name,
    Environment const& environment,
    std::string const& name,
    std::string const& description):

    _callable{std::forward<Callable>(callable)},
    _system_name{system_name},
    _environment{environment},
    _name{name},
    _description{description},
    _time_interval{},
    _timings{}

{
}


template<
    typename Callable>
std::string const& Benchmark<Callable>::system_name() const
{
    return _system_name;
}


template<
    typename Callable>
std::string const& Benchmark<Callable>::name() const
{
    return _name;
}


template<
    typename Callable>
std::string const& Benchmark<Callable>::description() const
{
    return _description;
}


template<
    typename Callable>
Environment const& Benchmark<Callable>::environment() const
{
    return _environment;
}


/*!
    @brief      Run the benchmarks @a count times
    @return     EXIT_SUCCESS or EXIT_FAILURE

    You can call this function multiple times, but any previous results
    will be overwritten in that case.

    After calling this function, @a count timings will be available.
*/
template<
    typename Callable>
inline int Benchmark<Callable>::run()
{
    _timings.clear();
    Stopwatch stopwatch;

    // Stream << _name << ": ";  // << std::flush;

    auto const start = Stopwatch::now();
    for(std::size_t i = 0; i < _environment.count(); ++i) {
        stopwatch.start();
        _callable(_environment);
        stopwatch.stop();
        _timings.push_back(Timing{stopwatch.time_interval()});
        // Stream << ".";  // << std::flush;
    }
    // Stream << "\n";  // std::endl;
    auto const stop = Stopwatch::now();

    _time_interval = TimeInterval{start, stop};

    return EXIT_SUCCESS;
}


/*!
    @brief      Return the time interval the experiment ran
*/
template<
    typename Callable>
inline TimeInterval const& Benchmark<Callable>::time_interval() const
{
    return _time_interval;
}


/*!
    @brief      Return the timings of each run
*/
template<
    typename Callable>
inline typename Benchmark<Callable>::Timings const&
    Benchmark<Callable>::timings() const
{
    return _timings;
}

}  // namespace benchmark
}  // namespace lue
