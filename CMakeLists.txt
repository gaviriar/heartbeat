# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)
project (Heartbeat)

#include files
set(PROJECT_PATH                               "${CMAKE_CURRENT_SOURCE_DIR}")
set(EXTERNAL_PATH                              "/home/unitelabs/projects/libwebrtc/_build")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}     "${PROJECT_PATH}/cmake")

if(NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++14 -pthread")
endif()

OPTION(BUILD_TESTS                             "Build tests"                    OFF )
OPTION(BUILD_DEPENDENCIES                      "Force build of dependencies"    OFF)
#include(Dependencies.cmake)
find_package(OpenCV REQUIRED)
#find_package(WebRTC REQUIRED)
find_package(g3logger REQUIRED)

message("OpenCV Libs: ${OpenCV_LIBS}")
#message("WebRTC Libs: ${WebRTC_LIBS}")
message("G3LOG Libs: ${G3LOG_LIBRARIES}")

# Check if project name has been specified correctly.
if((NOT PROJECT_NAME))
  message(FATAL_ERROR "Project name must be specified!")
endif ((NOT PROJECT_NAME))
if(${PROJECT_NAME} MATCHES " ")
    message(FATAL_ERROR "Project name cannot contain spaces!")
endif(${PROJECT_NAME} MATCHES " ")

# Set the list of include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(${PROJECT_NAME}_SRC 
    "${CMAKE_CURRENT_SOURCE_DIR}/Baseline.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ArgParser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RPPG.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opencv.cpp"
        CustomSink.hpp)

set(MAIN_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
        CustomSink.hpp)

set(SERVER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/serveur.cpp")
set(CLIENT_SRC "${CMAKE_CURRENT_SOURCE_DIR}/client.cpp")

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SRC})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${G3LOG_LIBRARIES})
#add_dependencies(${PROJECT_NAME})
add_executable(${PROJECT_NAME}_main ${MAIN_SRC})
target_link_libraries(${PROJECT_NAME}_main ${PROJECT_NAME} ${OpenCV_LIBS} ${G3LOG_LIBRARIES})

add_executable(${PROJECT_NAME}_client ${CLIENT_SRC})
target_link_libraries(${PROJECT_NAME}_client ${OpenCV_LIBS})
add_executable(${PROJECT_NAME}_server ${SERVER_SRC})
target_link_libraries(${PROJECT_NAME}_server ${OpenCV_LIBS})

# Build Tests
if(BUILD_TESTS)
  enable_testing()
  set(CMAKE_CURRENT_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/test)

  set(TEST_SRC)

  add_executable(test_${PROJECT_NAME} ${TEST_SRC})
  # is this necessary I wonder? 
  if(NOT CATCH_FOUND)
    message("-=-=-=-=-=-=-=-=-=-=-")
    add_dependencies(test_${PROJECT_NAME} catch-lib)
  endif(NOT CATCH_FOUND)

  message("${CMAKE_MODULE_PATH}")
  target_link_libraries(test_${PROJECT_NAME} ${PROJECT_NAME})

  # @todo what exactly does this function do?
  add_test(NAME test_${PROJECT_NAME} COMMAND "${PROJECT_BINARY_DIR}/test/test_${PROJECT_NAME}")
endif(BUILD_TESTS)
