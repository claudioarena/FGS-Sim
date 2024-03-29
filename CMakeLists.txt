cmake_minimum_required(VERSION 3.0.0)
set (CMAKE_CXX_STANDARD 20)

project (TwinkleSIM)
#enable_testing()
set (SOURCE
src/Frame.cpp 
src/FrameProcessor.cpp 
src/Test.cpp 
src/astroUtilities.cpp
src/PSF.cpp 
src/Brownian.cpp
src/MonteCarlo.cpp
src/PSF.cpp)
set(EXECUTABLE_OUTPUT_PATH ../bin/${CMAKE_BUILD_TYPE})

set (SOURCE_2
${SOURCE}
src/test_latexExample.cpp)
set (SOURCE_MAIN
${SOURCE}
src/main.cpp)

#add_executable (${PROJECT_NAME} ${SOURCE})
include_directories(src)
include_directories(include)
#TODO: RE-ENABLE optimization
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g -ggdb -Wall -Wextra")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g -ggdb -Wall -pedantic -Wextra")

#file(GLOB SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_MAIN})
add_executable(${PROJECT_NAME}_latexTest ${SOURCE_2})

#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)

enable_testing()
project(basic_test)
#TODO: RE-ENABLE optimization
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g -ggdb -Wall")

set(googleTest_Dir ${CMAKE_CURRENT_SOURCE_DIR}/include/googletest)
if (EXISTS ${googleTest_Dir})
    SET(GTestSrc ${googleTest_Dir}/googletest)
    SET(GMockSrc ${googleTest_Dir}/googlemock)
else ()
    message(FATAL_ERROR "No googletest src dir found!")
endif ()

include_directories(${GTestSrc} ${GTestSrc}/include ${GMockSrc} ${GMockSrc}/include)
include_directories(${CMAKE_HOME_DIRECTORY} ${CMAKE_HOME_DIRECTORY}/test)

set(test_HEADERS
    src/typedefs.h
)
set(test_SOURCES
    test/tests.cpp
    test/Frame_tester.cpp
    test/FrameProcessor_tester.cpp
    src/Frame.cpp
    src/Test.cpp
    src/astroUtilities.cpp
    src/PSF.cpp
    src/Brownian.cpp
    src/FrameProcessor.cpp
    src/MonteCarlo.cpp
    src/Brownian.cpp
    src/PSF.cpp
)

add_executable(unitTest ${test_SOURCES} ${test_HEADERS} ${GTestSrc}/src/gtest-all.cc ${GMockSrc}/src/gmock-all.cc)

#target_link_libraries(TwinkleFGS_test TwinkleFGS_test)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
add_test( runUnitTests runUnitTests )
