#
# Generic-gcc-da1458x configuration.
#

# some wxtra warnings by default
#set(_warnings "-Wimplicit -Wnested-externs -Wcast-align -Wpointer-arith -Wredundant-decls -Wshadow -Wcast-qual -Wcast-align")
# -Wextra -Wall

SET(CMAKE_C_FLAGS_INIT "-mcpu=cortex-m0 -mthumb -std=gnu99  -ffunction-sections -fdata-sections ${_warnings}")
SET(CMAKE_C_FLAGS_DEBUG_INIT "-g")
SET(CMAKE_C_FLAGS_MINSIZEREL_INIT "-Os -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE_INIT "-O2 -DNDEBUG")
SET(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")

# Shared libraries not supportted.
UNSET(CMAKE_C_CREATE_SHARED_LIBRARY )

# needed to get try_compile() to work
SET(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=nosys.specs")
# as default use nano-newlib
SET(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL_INIT "--specs=nano.specs")
