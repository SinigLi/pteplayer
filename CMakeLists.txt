cmake_minimum_required(VERSION 3.16)


set(BOOST_INCLUDE_PATH "" CACHE PATH "Path to user-defined include files (REQUIRED)")

if(BOOST_INCLUDE_PATH STREQUAL "")
    message(FATAL_ERROR "BOOST_INCLUDE_PATH must be set to ! Please configure with -BOOST_INCLUDE_PATH=/include/boost/include")
endif()

project( pteplayer )
# add_subdirectory( testcmake )
add_subdirectory( third_part/pugi )
add_subdirectory( third_part/minizip-tools )

add_subdirectory( third_part/rtmidi )

# if(ANDROID)
#     add_subdirectory( third_part/others/android/alsa )
# endif(ANDROID)


add_subdirectory( actions )
add_subdirectory( app )
add_subdirectory( audio )
add_subdirectory( dialogs )
add_subdirectory( formats )
add_subdirectory( midi )
add_subdirectory( painters )
add_subdirectory( score )
add_subdirectory( widgets )
add_subdirectory( util )

add_subdirectory( exec )


