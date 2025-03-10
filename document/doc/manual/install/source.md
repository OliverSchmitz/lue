(install-source)=

# Install from source

The LUE source code can be obtained from the [LUE GitHub project
page](https://github.com/computationalgeography/lue). CMake is used to configure and generate project files
for building LUE.

% TODO General, use of CMake, configure, build install

The following CMake variables are used to determine which targets to build:

| Variable                      | Default | Effect                          |
| ---                           | ---     | ---                             |
| `LUE_BUILD_DATA_MODEL`        | `TRUE`  | Build data model C++ API        |
| `LUE_BUILD_FRAMEWORK`         | `TRUE`  | Build modelling framework       |
| `LUE_BUILD_VIEW`              | `TRUE`  | Build data model viewer         |
| `LUE_BUILD_DOCUMENTATION`     | `FALSE` | Build API documentation         |
| `LUE_BUILD_QUALITY_ASSURANCE` | `FALSE` | Build quality assurance targets |

Here is an example session of building the default LUE targets and installing them in `$HOME/lue_install`:

```bash
cd /tmp
git clone https://github.com/computationalgeography/lue.git
cd lue
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$HOME/lue_install ..
cmake --build . --target install
```

Also, you can check the [LUE GitHub workflow
scripts](https://github.com/computationalgeography/lue/tree/master/.github/workflows) behind the [LUE GitHub
C/I actions](https://github.com/computationalgeography/lue/actions) to see how LUE can be built on various
platforms.


## Targets

### Data model

Optional features:

| Variable                         | Default | Effect |
| ---                              | ---     | --- |
| `LUE_DATA_MODEL_WITH_PYTHON_API` | `FALSE` | Build data model Python API |
| `LUE_DATA_MODEL_WITH_UTILITIES`  | `FALSE` | Build command line utilities |


#### Dependencies

- HDF5 C, Boost, fmt
- `LUE_DATA_MODEL_WITH_PYTHON_API`: Python, pybind11, NumPy, Guidelines Support libraries
- `LUE_DATA_MODEL_WITH_UTILITIES`: Docopt, GDAL, Nlohmann JSON


### Framework

Optional features:

| Variable                        | Default | Effect |
| ---                             | ---     | --- |
| `LUE_FRAMEWORK_WITH_PYTHON_API` | `FALSE` | Build modelling framework Python API |


#### Dependencies

- Boost, Docopt, fmt, GDAL, HPX, LUE data model, Kokkos mdspan
- `LUE_FRAMEWORK_WITH_PYTHON_API`: Python, pybind11, NumPy


### View

#### Dependencies

- Dear ImGUI, Docopt, fmt, GLFW, Nlohmann JSON, OpenGL


### Documentation

#### Dependencies

- Breathe, Doxygen, Graphviz, Jupyter-Book, Sphinx


### Quality assurance

Optional features:

| Variable                                             | Default | Effect                              |
| ---                                                  | ---     | ---                                 |
| `LUE_QUALITY_ASSURANCE_WITH_PYTHON_API`              | `FALSE` | Build quality assurance Python API  |
| `LUE_QUALITY_ASSURANCE_WITH_TESTS`                   | `FALSE` | Build test targets                  |
| `LUE_QUALITY_ASSURANCE_TEST_NR_LOCALITIES_PER_TEST`  | `1`     | Number of localities to use         |
| `LUE_QUALITY_ASSURANCE_TEST_NR_THREADS_PER_LOCALITY` | `1`     | Number of threads per locality      |
| `LUE_QUALITY_ASSURANCE_TEST_HPX_RUNWRAPPER`          | `none`  | Runwrapper to use (see `hpxrun.py`) |
| `LUE_QUALITY_ASSURANCE_TEST_HPX_PARCELPORT`          | `tcp`   | Parcelport to use (see `hpxrun.py`) |

```{note}
Changing `LUE_QUALITY_ASSURANCE_TEST_NR_LOCALITIES_PER_TEST` and
`LUE_QUALITY_ASSURANCE_TEST_NR_THREADS_PER_LOCALITY` is only relevant for tests that depend on the HPX
library. Using multiple localities (`LUE_QUALITY_ASSURANCE_TEST_NR_LOCALITIES_PER_TEST` > 1), requires that
HPX is built with support for networking.
```

#### Dependencies

- Nlohmann JSON
- `LUE_QUALITY_ASSURANCE_WITH_TESTS`: Boost filesystem, system, unit-test framework libraries
- `LUE_QUALITY_ASSURANCE_WITH_PYTHON_API`: pybind11, Python


## Dependencies

There are at least three options for installing prerequisite libraries and tools:

1. Install using platform specific package manager
1. Install using Conan, either before building LUE or by the LUE build scripts
1. Build by the LUE build scripts

The next variables determine what happens in each case by default.

`LUE_HAVE_FOO`:

- TRUE: Search for Foo in standard locations
- FALSE: Install Foo using Conan

| Variable                 | WIN32 | APPLE | Other |
| ---                      | ---   | ---   | ---   |
| `LUE_HAVE_BOOST`         | FALSE | TRUE  | TRUE  |
| `LUE_HAVE_DOXYGEN`       | FALSE | TRUE  | TRUE  |
| `LUE_HAVE_GDAL`          | FALSE | TRUE  | TRUE  |
| `LUE_HAVE_FMT`           | FALSE | TRUE  | TRUE  |
| `LUE_HAVE_GLFW`          | FALSE | TRUE  | TRUE  |
| `LUE_HAVE_HDF5`          | FALSE | TRUE  | TRUE  |
| `LUE_HAVE_MS_GSL`        | FALSE | FALSE | FALSE |
| `LUE_HAVE_NLOHMANN_JSON` | FALSE | TRUE  | FALSE |
| `LUE_HAVE_PYBIND11`      | FALSE | FALSE | FALSE |

Some prerequisite libraries can optionally be built by the LUE project
itself. The following variables are used to decide whether or not that
is the case. By default, they are not set.

| Variable         | Effect when set |
| ---              | --- |
| `LUE_BUILD_HPX`  | Build the HPX library |
| `LUE_BUILD_OTF2` | Build the OTF2 library (optionally required by HPX) |
