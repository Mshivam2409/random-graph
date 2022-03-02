cmake_minimum_required(VERSION 3.0.0)
project(randomizedGraph VERSION 0.1.0)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_COMPILER /usr/bin/gcc-10 CACHE PATH "" FORCE)
set(CMAKE_CXX_COMPILER /usr/bin/g++-10 CACHE PATH "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

include(CTest)
enable_testing()

include_directories("include")
file(GLOB SOURCES "src/*.cpp")
add_executable(randomizedGraph src/main.cpp  src/random_graph.cpp)


set_property (TARGET ${PROJECT_NAME}
  PROPERTY
    CXX_STANDARD 20
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(randomizedGraph  PRIVATE Threads::Threads)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(randomizedGraph PUBLIC OpenMP::OpenMP_CXX)
    message(STATUS "Using OpenMP ${OPENMP_CXX_VERSION}")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -Ofast")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -Ofast")
endif()

find_package(PkgConfig REQUIRED)
pkg_search_module(OPENSSL REQUIRED openssl)
if( OPENSSL_FOUND )
    include_directories(${OPENSSL_INCLUDE_DIRS})
    message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
endif()

target_link_libraries(randomizedGraph PUBLIC ${OPENSSL_LIBRARIES})

find_package(ZLIB)
if(ZLIB_FOUND)
include_directories(${ZLIB_INCLUDE_DIRS})
target_link_libraries(randomizedGraph PUBLIC ZLIB::ZLIB)
endif()


set(CXX_FILESYSTEM_LIBRARIES "stdc++fs")
target_link_libraries(randomizedGraph PUBLIC ${CXX_FILESYSTEM_LIBRARIES})

find_package(PythonLibs 2.7 REQUIRED)
target_include_directories(randomizedGraph PRIVATE ${PYTHON_INCLUDE_DIRS})
target_link_libraries(randomizedGraph PRIVATE ${PYTHON_LIBRARIES})

target_include_directories(randomizedGraph PRIVATE lib/taskflow/taskflow)
target_include_directories(randomizedGraph PRIVATE lib/stats/include)
add_compile_definitions(STATS_ENABLE_STDVEC_WRAPPERS)
target_include_directories(randomizedGraph PRIVATE lib/gcem/include)
target_include_directories(randomizedGraph PRIVATE lib/indicators/include)
target_include_directories(randomizedGraph PRIVATE /usr/include/c++/10)
target_include_directories(randomizedGraph PRIVATE lib/CSVWriter/include)
target_include_directories(randomizedGraph PRIVATE lib/matplotlib-cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
