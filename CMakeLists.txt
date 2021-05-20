project(singleprocessmultiprocess)
set(CMAKE_CXX_STANDARD 17)
add_subdirectory(third_party/fmt)


find_package (Python COMPONENTS Development)
add_library(python_runner SHARED src/python_runner.cpp)
target_include_directories(python_runner PRIVATE third_party/pybind11/include ${Python_INCLUDE_DIRS})

add_library(find_shared_function SHARED src/find_shared_function.cpp)
target_link_libraries(find_shared_function PRIVATE fmt::fmt-header-only)

add_executable(main src/main.cpp src/loader.cpp)
target_compile_definitions(main PRIVATE PYTHON_SO_PATH=\"${Python_LIBRARIES}\")
target_link_libraries(main PRIVATE dl pthread fmt::fmt-header-only)
