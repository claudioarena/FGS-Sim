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

project (unitTest)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE include/catch)

set (SOURCE test/catch_test_2.cpp test/catch_test.cpp)
add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} Catch)

include_directories(test)
include_directories(include)
#file(GLOB SOURCES "src/*.cpp")
#add_executable(${PROJECT_NAME} ${SOURCE})
