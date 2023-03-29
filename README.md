# yet-another-ping-tool (yapt)

# Easily run ping against multiple targets

Includes support for unit testing via GoogleTest library as well as boost library to get you up and running quickly.

Focus on:

1. Easy to understand 
1. Debuggability
1. Optimizable
1. Extensible


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
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1
```



If you need to provide customizations for GCC you can use:

```
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=`pwd`/toolchains/gcc.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1
```

1. Build using cmake

```
cmake --build build
```

Ensure that binary has correct capabilities set:

```
sudo setcap 'cap_net_raw=ep' build/main
```

Run binary:

```
./build/main -ips_file=sampleIPs.lst --logtostderr=1
```

# Generate GRPC files

```
vcpkg_installed/x64-linux/tools/protobuf/protoc --grpc_out=protos --plugin=protoc-gen-grpc=vcpkg_installed/x64-linux/tools/grpc/grpc_cpp_plugin service.proto
```

# Memory profiling

Needed tools:

1. jeprof
1. Flamegraph

```
sudo /bin/bash -c 'MALLOC_CONF="prof:true,prof_active:false,lg_prof_interval:2,prof_prefix:jeprof.out" ./build/yapt -ips_file=sampleIPs.lst --max_log_size=32 --logtostderr=1'
```

Convert captured profile to a format usable by flamegraph tool:

```
jeprof --collapsed build/yapt jeprof.out.357942.0.i0.heap > jeprof.out.357942.0.i0.heap.collapsed
```

Generate flamegraph:

```
~/tools/FlameGraph/flamegraph.pl jeprof.out.374693.0.i0.heap.collapsed > jeprof.out.374693.0.i0.heap.svg
```

Script to convert all files ending in *.heap in current directory to flamegraphs:

```
for f in *.heap; do jeprof --collapsed build/yapt $f > $f.collapsed; ~/tools/FlameGraph/flamegraph.pl $f.collapsed > $f.svg; done
```


# Testing
1. run `main_test` binary

# Formatting

```
clang-format -style=google -dump-config > .clang-format
```


# Investigating coredumps:

1. Find coredump that you'd like to look into:

```
sudo coredumpctl list yapt
sudo coredumpctl debug xxxxx
```