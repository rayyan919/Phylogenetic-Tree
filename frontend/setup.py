# # setup.py
# # This file builds the Python extension module for your C++ backend

# from setuptools import setup, Extension
# import pybind11

# # Define the extension module
# ext_modules = [
#     Extension(
#         'backend_bridge',  # Name of the extension module
#         ['backend_bridge.cpp', 'backend.cpp'],  # Source files
#         include_dirs=[pybind11.get_include()],  # Include directories
#         language='c++',
#         extra_compile_args=['-std=c++11'],  # Use C++11 standard
#     ),
# ]

# # Setup the module
# setup(
#     name='backend_bridge',
#     version='0.1',
#     ext_modules=ext_modules,
#     install_requires=['pybind11>=2.6.0'],
# )


from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'backend_bridge',  # name of the Python module
        ['backend_bridge.cpp'],  # C++ source file
        include_dirs=[pybind11.get_include()],  # pybind11 include directory
        language='c++',
    ),
]

setup(
    name='backend_bridge',
    ext_modules=ext_modules,
)
