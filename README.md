# life-of-boids in C++

## Requirements

### Install conan

```
pip3 install conan
```

### Configure conan
* For GCC
  ```
  conan profile new default --detect
  conan profile update settings.compiler=gcc default
  conan profile update settings.compiler.version="${VERSION}" default
  conan profile update settings.compiler.libcxx=libstdc++11 default
  export CXX=g++${VERSION} 
  export CC=gcc${VERSION}
  ```
* For Clang
  ```
  conan profile new default --detect
  conan profile update settings.compiler=clang default
  conan profile update settings.compiler.version="${VERSION}" default
  conan profile update settings.compiler.libcxx=libstdc++11 default
  export CXX=clang++${VERSION} 
  export CC=clang${VERSION}
  ```
* For AppleClang
  ```
  conan profile new default --detect
  ```
* For MSVC
  ```
  conan profile new default --detect
  ```

## Build and tests

Choose a build mode
```
MODE=Release
```
or
```
MODE=Debug
```

### Unix like (Linux ou macOS)

It will use `CC` and `CXX` environment variables defined before.

```
mkdir build
cd build
conan install ..
cmake \
    -DCMAKE_TOOLCHAIN_FILE=conan_paths.cmake \
    -DCMAKE_BUILD_TYPE="${MODE}" \
    ..
cmake --build .
ctest 
```

### Windows
```
mkdir build
cd build
conan install ..
cmake \
    -DCMAKE_TOOLCHAIN_FILE=conan_paths.cmake \
    -DCMAKE_GENERATOR_PLATFORM=x64 \
    ..
cmake --build . --config "${MODE}"
ctest -C "${MODE}"
```
