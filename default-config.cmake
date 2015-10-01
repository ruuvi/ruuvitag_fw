#
# project default configuration
# used to pre-populate cmake cache -> "cmake -C project-config.cmake"
# AND included from top level CMakeList.txt
#

set (PROJECT_NAME "RuuviTag")

# default build type: optimize for size
set (CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING "Build type")

set (CMAKE_TOOLCHAIN_FILE "toolchain-gcc-arm.cmake" CACHE STRING "Compiler toolchain to use")
