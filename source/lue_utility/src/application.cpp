#include "lue/utility/application.hpp"
// #include "lue_utility/print_message.h"
// #include "lue_utility/stopwatch.h"
// #include <hdf5.h>
// #include <boost/format.hpp>
// #include <cassert>
// #include <iostream>


namespace lue {
namespace utility {
namespace {

// void MyErrorHandler(
//     CPLErr /* eErrClass */,
//     int err_no,
//     char const* msg)
// {
//     std::cout << msg << std::endl;
//     assert(false);
// }

}  // Anonymous namespace


// template<>
// std::string Application::argument<std::string>(
//     std::string const& name) const
// {
//     assert(argument_passed(name));
//     return _arguments.at(name).asString();
// }
// 
// 
// template<>
// std::vector<std::string> Application::argument<std::vector<std::string>>(
//     std::string const& name) const
// {
//     assert(argument_passed(name));
//     return _arguments.at(name).asStringList();
// }
// 
// 
// template<>
// bool Application::argument<bool>(
//     std::string const& name) const
// {
//     assert(argument_passed(name));
//     return _arguments.at(name).asBool();
// }


Application::Application(
    std::string const& usage,
    std::vector<std::string> const& arguments)

    : Command(usage, arguments)
      // _info_stream(std::cout),
      // _error_stream(std::cerr),
      // _arguments(docopt::docopt(usage, { argv + 1, argv + argc }, true,
      //     "0.0.0"))  // ,
      // _verbose(argument<bool>("--verbose"))

{
    // print_arguments();

    // // CPLSetErrorHandler(MyErrorHandler);

    // if(!_verbose) {
        // Turn off error stack traversal. The default functions prints
        // lots of messages we normally don't care about.
//         H5Eset_auto1(NULL, NULL);
    // }
}


Application::Application(
    std::string const& usage,
    std::vector<std::string> const& arguments,
    SubCommands const& sub_commands)

    : Command(usage, arguments, sub_commands)

{
}


// void Application::print_info_message(
//     std::string const& message) const
// {
//     lue::utility::print_info_message(_info_stream, message);
// }
// 
// 
// void Application::print_error_message(
//     std::string const& message) const
// {
//     lue::utility::print_error_message(_error_stream, message);
// }


// void Application::print_verbose_message(
//     std::string const& message) const
// {
//     lue::utility::print_info_message(_verbose, _info_stream, message);
// }


// void Application::print_arguments()
// {
//     print_verbose_message("commandline arguments:");
// 
//     for(auto const& pair: _arguments) {
//         print_verbose_message((boost::format("    %1%: %2%")
//             % pair.first
//             % pair.second).str());
//     }
// }


// bool Application::argument_passed(
//     std::string const& name) const
// {
//     return static_cast<bool>(_arguments.find(name)->second);
// }


// int Application::run() noexcept
// {
//     int status = EXIT_FAILURE;
// 
//     try {
//         Stopwatch stopwatch;
//         stopwatch.start();
// 
//         try {
//             auto const command = argument<std::string>("<command>");
//             std::cout << command << std::endl;
//             // run_implementation();
//         }
//         catch(std::bad_alloc const& exception) {
//             print_error_message("not enough memory");
//         }
//         catch(std::exception const& exception) {
//             print_error_message(exception.what());
//         }
// 
//         stopwatch.stop();
//         // print_verbose_message("finished at: " + to_string(stopwatch.end()));
//         // print_verbose_message("elapsed time: " + std::to_string(
//         //     stopwatch.elapsed_seconds()) + "s");
// 
//         status = EXIT_SUCCESS;
//     }
//     catch(...) {
//         print_error_message("unknown error");
//     }
// 
//     // print_verbose_message("exit status: " + std::to_string(status));
// 
//     return status;
// }

}  // namespace utility
}  // namespace lue
