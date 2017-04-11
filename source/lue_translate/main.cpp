#include "lue_translate/translate.h"
#include <boost/format.hpp>


namespace {

// std::string const usage = boost::str(boost::format(R"(
// Translate data into or from the LUE dataset format
// 
// usage:
//     %1% <command> [<arguments>...]
//     lue_translate (-h | --help) | --version
// 
// options:
//     -h --help             Show this screen
//     --version             Show version
// 
// commands:
//     import      Import data into a LUE dataset
//     export      Export data from a LUE dataset
// 
// See '%1% help <command>' for more information on a command.
// )") % "lue_translate");


std::string const usage = R"(lue_translate

usage:
    lue_translate [--verbose] <input> <output>
    lue_translate [--verbose] --start=<time_point>
        --cell=<duration> <input> <output>
    lue_translate (-h | --help) | --version

options:
    -h --help             Show this screen
    --verbose             Show info messages
    --version             Show version
    --start=<time_point>  Time point of first slice
    --cell=<duration>     Duration of time step

Time points must be formatted according to the ISO-8601 standard.

Durations must be formatted according to the ISO-8601 standard. A duration
determines the resolution of the discretization, and the associated
time point must 'match' this resolution: it makes no sense to provide
a time point at a higher resolution than the duration. For example,
given a duration of months (P1M), the time point must be given as YYYY-MM
(and not YYYY-MM-DD, for example).
)";

} // Anonymous namespace


int main(
    int argc,
    char* argv[])
{
    return lue::utility::Translate(argc, argv, usage).run();
}
