set(PYBIND11_CPP_STANDARD -std=c++20)
# set(PYTHON_EXECUTABLE /usr/bin/python3)

include_directories(.. ../src/)

find_package(pybind11)

pybind11_add_module(pysamlib SHARED samlib.cc)
