# yet-another-ping-tool (yapt)

# Easily run ping against multiple targets

Includes support for unit testing via GoogleTest library as well as boost library to get you up and running quickly.


# Building

1. Clone repository
1. Update vcpkg submodule
1. Configure vcpkg

```
./vcpkg/vcpkg install
```

In order to use a different compiler (e.g. clang) or a specific version of GCC you can use custom triplets:

```
./vcpkg/vcpkg install --overlay-triplets=custom-triplets
```

```
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake
```

If you need to provide customizations for GCC you can use:

```
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=./toolchains/gcc.cmake
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
