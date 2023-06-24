# User's guide to Broma
Thank you for using Broma! This guide aims to give you a quick run-through of how to set up the library, and how to start using it.

# Requirements
Broma requires:
- **A C++17 compliant compiler.** Any recent version of Clang, GCC, or Microsoft Visual C++ should work.
- **CMake.** Although this may not be strictly necessary, we expect you to use CMake throughout this tutorial, to align with Broma's own build system.

# Installation
The easiest way to add Broma to your project is to use [CPM](https://github.com/cpm-cmake/CPM.cmake).
This also prevents code duplication, since Broma uses CPM internally to install its dependencies.
You can find out how to install CPM to your own CMake project [here](https://github.com/cpm-cmake/CPM.cmake#adding-cpm).

After installing CPM, add the following line to your CMake file to include Broma:
```cmake
CPMAddPackage("gh:camila314/Broma@1.0.0")
```

Then, add Broma as a dependency of your binary (or library):
```cmake
# My example project
add_executable(example example.cpp)

# Add Broma!
target_link_libraries(example PRIVATE Broma)
```

# Usage
First, include Broma as a header in your C++ file:
```cpp
#include <broma.hpp>
```

Then, call Broma's `parse_file` with your desired file path:
```cpp
// In some function...
// Broma throws on parse errors, so make sure to catch them if needed!
try {
    broma::Root root = broma::parse_file(/* Your filename! */);
}
catch (std::exception& const err) {
    std::cout << "Parse failed! " << err;
}
```

This should return a `broma::Root` struct, which contains all parsed classes and functions found.
You can iterate through each class (and function) like so:
```cpp
// In some function...
for (broma::Class& cls : root.classes) {
    std::string& name = cls.name;
    for (broma::Field& field : cls.fields) {
        // Since `field` is a std::variant, we need to cast it
        if (auto func = field.get_as<broma::FunctionBindField>()) {
            // Do something with the class's function...
        }
        else if (auto pad = field.get_as<broma::PadField>()) {
            // Do something with the padding...
        }
    }
}

for (broma::Function& func : root.functions) {
    // The parsed signature of the function.
    broma::FunctionProto proto = func.prototype;
    // The bound address of the function for each different supported platform.
    broma::PlatformNumber num_binds = func.binds;
}
```

See the declarations of `broma::Function` and `broma::Class` for more information.
Happy hacking!
