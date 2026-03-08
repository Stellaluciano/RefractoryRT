cmake_minimum_required(VERSION 3.14)
project(RefractoryRT LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(refractoryrt
  src/refractory_policy.cpp
  src/scheduler.cpp
  src/metrics.cpp
  src/simulator.cpp
  src/workload.cpp
)
target_include_directories(refractoryrt PUBLIC include)

add_executable(refractoryrt_cli src/main.cpp)
target_link_libraries(refractoryrt_cli PRIVATE refractoryrt)

add_executable(benchmark_bursty bench/benchmark_bursty.cpp)
target_link_libraries(benchmark_bursty PRIVATE refractoryrt)

add_executable(benchmark_mixed bench/benchmark_mixed.cpp)
target_link_libraries(benchmark_mixed PRIVATE refractoryrt)

add_executable(benchmark_tail bench/benchmark_tail.cpp)
target_link_libraries(benchmark_tail PRIVATE refractoryrt)

enable_testing()

add_executable(test_refractory_policy tests/test_refractory_policy.cpp)
target_link_libraries(test_refractory_policy PRIVATE refractoryrt)
add_test(NAME test_refractory_policy COMMAND test_refractory_policy)

add_executable(test_scheduler tests/test_scheduler.cpp)
target_link_libraries(test_scheduler PRIVATE refractoryrt)
add_test(NAME test_scheduler COMMAND test_scheduler)

add_executable(test_metrics tests/test_metrics.cpp)
target_link_libraries(test_metrics PRIVATE refractoryrt)
add_test(NAME test_metrics COMMAND test_metrics)
