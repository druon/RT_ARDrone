### CMake file for RT_ARDrone
###
###   -  2010: Initial version <druon@lirmm.fr>
###
###
###############################################################################


cmake_minimum_required (VERSION 2.6)

project (RT_ARDrone)
set (RT_ARDrone_VERSION_MAJOR 0)
set (RT_ARDrone_VERSION_MINOR 1)

set( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /lib /usr/lib /usr/local/lib )
set( CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /usr/include /usr/local/include )
set( CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  "${PROJECT_SOURCE_DIR}/cmake" ) 

set ( INSTALL_BIN_PATH bin )
set ( INSTALL_LIB_PATH lib )
set ( INSTALL_AR_PATH  lib )
set ( INSTALL_HEADERS_PATH include )


# Make relative paths absolute (needed later on)

foreach(p LIB BIN HEADERS AR)
    set(var INSTALL_${p}_PATH)
    if(NOT IS_ABSOLUTE "${${var}}")
        set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
    endif()
endforeach()


### Search for Lib/Headers

find_package( Threads )


### Configure a header file to pass some of the CMake settings
### to the source code

configure_file (
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_SOURCE_DIR}/include/RT_ARDrone/config.h"
)


### Set include directories 
        
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/include"
	             "${CMAKE_CURRENT_SOURCE_DIR}/src"
        	     "${CMAKE_CURRENT_BINARY_DIR}"
                     "${CMAKE_INSTALL_PREFIX}/include" )


### Set link directories

# link_directories( "${Boost_LIBRARY_DIRS}" )


### File Lists

file( GLOB hdr_global  "include/RT_ARDrone/*.h*")
file( GLOB hdr "src/*.h*" )
file( GLOB src "src/*.cpp" "src/*.c" )



### Main Target and linking

add_library( "RT_ARDrone" SHARED ${src} ${hdr} )
target_link_libraries ( "RT_ARDrone" ${CMAKE_THREAD_LIBS_INIT})

### Tests programs

add_subdirectory( "test" )

### Install targets

install( TARGETS "RT_ARDrone"
        RUNTIME DESTINATION ${INSTALL_BIN_PATH}  COMPONENT main
        LIBRARY DESTINATION ${INSTALL_LIB_PATH}  COMPONENT main
        ARCHIVE DESTINATION ${INSTALL_AR_PATH}   COMPONENT main )


install( FILES ${hdr_global}
        DESTINATION ${INSTALL_HEADERS_PATH}/RT_ARDrone/${name}
        COMPONENT main )

### Configure file

configure_file(RT_ARDroneConfig.cmake.in "${PROJECT_BINARY_DIR}/RT_ARDroneConfig.cmake" @ONLY)

install ( FILES "${PROJECT_BINARY_DIR}/RT_ARDroneConfig.cmake" 
      DESTINATION "${INSTALL_LIB_PATH}/RT_ARDrone" COMPONENT dev )  
        

