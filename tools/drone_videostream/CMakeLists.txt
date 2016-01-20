### CMake file for RT_ARDrone_VideoStream
###
###   -  2010: Initial version <druon@lirmm.fr>
###
###
###############################################################################


cmake_minimum_required (VERSION 2.6)

project (RT_ARDrone_VideoStream)
set (RT_ARDrone_VideoStream_VERSION_MAJOR 0)
set (RT_ARDrone_VideoStream_VERSION_MINOR 1)

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

find_package( RT_ARDrone REQUIRED )
find_package( OpenGL )
find_package( X11 )
	
### Set include directories 
        
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/include"
	             "${CMAKE_CURRENT_SOURCE_DIR}/src"
        	     "${CMAKE_CURRENT_BINARY_DIR}"
                     "${CMAKE_INSTALL_PREFIX}/include"
		     "${RT_ARDRONE_INCLUDE_DIRS}"
        	     "${X11_X11_INCLUDE_PATH}"
		     "${OPENGL_INCLUDE_DIR}"
)


### Add library path

LINK_DIRECTORIES( ${LINK_DIRECTORIES} ${RT_ARDRONE_LIBRARY_DIR} )

### File Lists

file( GLOB hdr "src/*.h*" )
file( GLOB src "src/*.cpp" "src/*.c" )


### Main Target and linking

add_executable( "RT_ARDrone_VideoStream" ${src} ${hdr} )

target_link_libraries ( "RT_ARDrone_VideoStream" ${CMAKE_THREAD_LIBS_INIT}
					${RT_ARDRONE_LIBRARIES} 
					${SDL_LIBRARY} 
				        ${X11_X11_LIB} 
					${OPENGL_gl_LIBRARY} "Xxf86vm" )
### Install targets

install( TARGETS "RT_ARDrone_VideoStream"
        RUNTIME DESTINATION ${INSTALL_BIN_PATH}  COMPONENT main
        LIBRARY DESTINATION ${INSTALL_LIB_PATH}  COMPONENT main
        ARCHIVE DESTINATION ${INSTALL_AR_PATH}   COMPONENT main )


        

