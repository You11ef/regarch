cmake_minimum_required(VERSION 3.20)
project(RegArchWrapper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# IGNORING LNK4099 WARNING
if (MSVC)
    # Add the /ignore:4099 flag to suppress LNK4099 warnings
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /ignore:4099")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /ignore:4099")
endif()

# 1) Check environment variables
if(NOT DEFINED ENV{REGARCH_HOME})
  message(FATAL_ERROR "REGARCH_HOME environment variable is not set.")
endif()

# 2) Set runtime library for MSVC
if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# 3) Include directories
include_directories(
  $ENV{REGARCH_HOME}/Include
  $ENV{REGARCH_HOME}/Include/gsl-include
  $ENV{REGARCH_HOME}/Include/nlopt-include
  $ENV{BOOST_HOME}
  $ENV{PYTHON_HOME}/include
)

# 4) Link directories (per-configuration)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  link_directories(
    "$ENV{REGARCH_HOME}/Lib/Debug/x64"
    "$ENV{REGARCH_HOME}/Lib/gsl-lib/Debug/x64/v143"
    "$ENV{REGARCH_HOME}/Lib/nlopt-lib/Debug/x64/v143"
  )
else()
  link_directories(
    "$ENV{REGARCH_HOME}/Lib/Release/x64"
    "$ENV{REGARCH_HOME}/Lib/gsl-lib/Release/x64/v143"
    "$ENV{REGARCH_HOME}/Lib/nlopt-lib/Release/x64/v143"
  )
endif()

link_directories(
  $ENV{BOOST_HOME}/stage/lib
  $ENV{PYTHON_HOME}/libs
)

# 5) Disable Boost auto-linking
add_definitions(-DBOOST_ALL_NO_LIB -DBOOST_PYTHON_STATIC_LIB)

# 6) Create targets
add_executable(TestRegArch
  src/main.cpp
  python_wrapper/Wrap_cAbstCondMean.cpp
  python_wrapper/Wrap_cError.cpp
  python_wrapper/Wrap_cGSLVector.cpp
  python_wrapper/Wrap_cGSLMatrix.cpp
 python_wrapper/Wrap_cTabOfString.cpp)

add_library(regarch_wrapper SHARED
  src/RegArchPyWrapper.cpp
  python_wrapper/Wrap_cAbstCondMean.cpp
  python_wrapper/Wrap_cError.cpp
  python_wrapper/Wrap_cGSLVector.cpp
  python_wrapper/Wrap_cGSLMatrix.cpp
 python_wrapper/Wrap_cTabOfString.cpp)
set_target_properties(regarch_wrapper PROPERTIES SUFFIX ".pyd")

# 7) Link libraries (generalize Boost name)
target_link_libraries(TestRegArch PRIVATE
  RegArchLib VectorAndMatrix Error gsl cblas nlopt
  WrapperGslCpp WrapperNloptCpp
  libboost_python311-vc143-mt-sgd-x64-1_87
  $ENV{PYTHON_HOME}/libs/python311.lib
)

target_link_libraries(regarch_wrapper PRIVATE
  RegArchLib VectorAndMatrix Error gsl cblas nlopt
  WrapperGslCpp WrapperNloptCpp
  libboost_python311-vc143-mt-sgd-x64-1_87
  $ENV{PYTHON_HOME}/libs/python311.lib
)

target_compile_definitions(regarch_wrapper PRIVATE
  _GSL_ _NLOPT_ _USING_NAMESPACE_
)