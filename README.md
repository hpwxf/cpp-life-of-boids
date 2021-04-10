# life-of-boids C++ part

See https://github.com/hpwxf/rs-life-of-boids for Rust part.

## This version includes profiler experiments
Select the profiler in cmake command

# Tracy
```
git clone https://github.com/wolfpld/tracy.git
```
* to build its GUI on macOS, use:
```
brew install freetype capstone gtk glfw
make -C profiler/build/unix release
```
* to build its GUI on Linux Ubuntu 20.02, use:
```
apt install libcapstone-dev libtbb-dev libglfw3-dev libfreetype6-dev libgtk-3-dev
make -C profiler/build/unix release
```

# Easy Profiler
```
git clone https://github.com/yse/easy_profiler
```
* to build its GUI on macOS, use:
```
cmake -DCMAKE_PREFIX_PATH=/usr/local/Cellar/qt@5/5.15.2 -DCMAKE_CXX_COMPILER=g++-9 -DCMAKE_C_COMPILER=gcc-9 -DCMAKE_BUILD_TYPE="Release" ..
```

## Build

```
mkdir build
cd build
conan install ../..
cmake -DCMAKE_TOOLCHAIN_FILE=conan_paths.cmake -DENABLE_TRACY_PROFILER=false .. # ... or true
```

## CMake options

* `-DENABLE_STATIC_ANALYSIS=ON|OFF` : enable/disable static analysis while compiling

* `-DCMAKE_CXX_COMPILER_LAUNCHER=ccache` : enable `ccache` as compiler cache
