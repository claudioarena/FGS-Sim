cmake_minimum_required(VERSION 3.0.0)
set (CMAKE_CXX_STANDARD 20)

project (TwinkleSIM)
#enable_testing()
set (SOURCE src/main.cpp src/Frame.cpp src/FrameProcessor.cpp src/Test.cpp src/PSF.cpp src/Brownian.cpp)
set(EXECUTABLE_OUTPUT_PATH ../bin/${CMAKE_BUILD_TYPE})
#add_executable (${PROJECT_NAME} ${SOURCE})
include_directories(src)
include_directories(include)
#file(GLOB SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCE})

#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)

project (unitTests)
include(CTest)
#set(Boost_DEBUG ON)
#set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_RUNTIME OFF)

set(BOOST_ROOT "C:/local/boost_1_67_0")
set(BOOST_INCLUDEDIR "C:/local/boost_1_67_0")
set(BOOST_LIBRARYDIR  "C:/local/boost_1_67_0/lib64-msvc-14.1")
set(Boost_COMPILER  "-vc141")
set(Boost_ARCHITECTURE  "-x64")
add_definitions(-DBOOST_ALL_NO_LIB)
add_definitions(-DBOOST_ALL_DYN_LINK)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

set (SOURCE ./test/main-static.cpp
./test/tests.cpp)
add_executable(${PROJECT_NAME} ${SOURCE})
#target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_INCLUDE_DIRS})

#target_link_libraries(${PROJECT_NAME} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(${PROJECT_NAME} 
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                      )

include_directories(${Boost_INCLUDE_DIRS})
include_directories(src)
#include_directories(test)
include_directories(include)

add_test(runall ${PROJECT_NAME} )





enable_testing()
project(basic_test)

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
        )
set(test_SOURCES
        test/tests.cpp
        )

add_executable(unitTest ${test_SOURCES} ${GTestSrc}/src/gtest-all.cc ${GMockSrc}/src/gmock-all.cc)

#target_link_libraries(TwinkleFGS_test TwinkleFGS_test)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
add_test( runUnitTests runUnitTests )
