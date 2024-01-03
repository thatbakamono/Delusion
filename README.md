# Delusion

[![CI](https://github.com/thatbakamono/Delusion/actions/workflows/ci.yml/badge.svg)](https://github.com/thatbakamono/Delusion/actions/workflows/ci.yml)

Delusion is a 2D game engine written in C++ 20.

## Features

### Graphics

- [x] Sprites

Currently the only supported image format for textures is .png but [the underlying library](https://github.com/image-rs/image) supports many more formats and it's planned to enable them in the near future.

### Physics

- Rigidbody physics:
  - [x] Density
  - [x] Friction
  - [x] Restitution  

- Collisions:
  - [x] Box colliders

### Audio

Delusion at the moment supports playback of one audio file at a time with no effects applied. It's planned to get rid of this limitation by implementing proper audio mixer and making an API for implementing custom DSP effects.

Currently the only supported audio format is .mp3 but [the underlying library](https://github.com/ffmpeg/ffmpeg) supports lots of different formats and it's planned to enable most of the reasonable ones in the future.

### Scripting

Delusion supports scripting in C# (and F#, VB.Net too) via [the legacy mono](https://github.com/mono/mono). This means that the latest supported target is .NET Framework 4.8. It's planned to upgrade to [the latest mono](https://github.com/dotnet/runtime/tree/main/src/mono) to support .NET 8.

### Editor

Delusion has a fully functional editor, supporting all features supported by the engine.

## Build requirements

- CMake 3.26.0 or newer.
- MSBuild, ninja or make.
- Preferably the latest version of MSVC. While it might not be strictly necessary to use **the latest** version, delusion uses some features from C++ 20 and will use even more in the feature. Older versions might not support those features.
- Rust v1.64.0 or newer.
- MSBuild and C# compiler able to compile code written for .NET Framework 4.8.

## Build instructions

**Before proceeding, make sure you cloned delusion recursively via --recursive, delusion uses submodules and won't compile without them**

### Windows

#### Using MSBuild

```sh
Delusion> mkdir build
Delusion> cd build
Delusion> cmake -G "Visual Studio 17 2022" ..
Delusion> cmake --build . --config Release
```

#### Using ninja

```sh
Delusion> mkdir build
Delusion> cd build
Delusion> cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
Delusion> cmake --build .
```

#### Using make

##### MSYS variant

```sh
Delusion> mkdir build
Delusion> cd build
Delusion> cmake -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release ..
Delusion> cmake --build .
```

##### MinGW variant

```sh
Delusion> mkdir build
Delusion> cd build
Delusion> cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
Delusion> cmake --build .
```

### Other platforms

Other platforms are not officially supported at the moment. 
