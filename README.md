# yet-another-ping-tool (yapt)

# Easily run ping against multiple targets

Includes support for unit testing via GoogleTest library as well as boost library to get you up and running quickly.


# Building

1. Clone repository
1. Update vcpkg submodule
1. Configure vcpkg

```
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake 
```
1. Build using cmake

```
cmake --build build
```


# Testing
1. run `main_test` binary

# Formatting

```
clang-format -style=google -dump-config > .clang-format
```
