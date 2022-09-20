INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_PHYAUTH PHYAuth)

FIND_PATH(
    PHYAUTH_INCLUDE_DIRS
    NAMES PHYAuth/api.h
    HINTS $ENV{PHYAUTH_DIR}/include
        ${PC_PHYAUTH_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    PHYAUTH_LIBRARIES
    NAMES gnuradio-PHYAuth
    HINTS $ENV{PHYAUTH_DIR}/lib
        ${PC_PHYAUTH_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/PHYAuthTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PHYAUTH DEFAULT_MSG PHYAUTH_LIBRARIES PHYAUTH_INCLUDE_DIRS)
MARK_AS_ADVANCED(PHYAUTH_LIBRARIES PHYAUTH_INCLUDE_DIRS)
