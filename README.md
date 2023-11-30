[toc] <!--  At some point it will be supported by Obsidian  -->

# SAGE: *Super Advanced Game Engine*

> [!TIP]
> `cmake --build build -- usage`

## Getting Started
```
cmake -B build -D SAGE_OPT_CCACHE_ENABLED=ON
```
To customize the build see [README#Build](README.md#build).

Once you have a successful build you can begin reading [bin/main.cpp](bin/main.cpp) and the tests to get a feel of the project.

## Development
A big part of SAGE is automated. Once you are done tweaking/extending/refactoring, make sure you run cmake at least once to generate docs.

### Build
```
cmake -B build	\
      -G Ninja	\
      -D SAGE_OPT_CCACHE_ENABLED=ON	\
      -D CMAKE_LINKER=mold"
```

| Option | Doc |
|:-|:-|
|`SAGE_OPT_CCACHE_ENABLED`| Use [`ccache`](https://ccache.dev/) to speedup subsequent compilations|
|`SAGE_OPT_OBSIDIAN`| Tell SAGE to use Gkiwnis' [.obsidian](https://obsidian.md/)|
|`SAGE_OPT_VERBOSE`| |

## Project Overview
```
.
├── asset
│   ├── CMakeLists.txt
│   ├── shader
│   │   ├── CMakeLists.txt
│   │   ├── flat_color.glsl
│   │   ├── some.glsl
│   │   ├── square.glsl
│   │   └── texture.glsl
│   └── texture
│       ├── CMakeLists.txt
│       ├── owl.png
│       └── Ship.png
├── bin
│   ├── CMakeLists.txt
│   ├── layer_2d.hpp
│   └── main.cpp
├── cmake
│   ├── assert.cmake
│   ├── ccache.cmake
│   ├── compiler.cmake
│   ├── convenience.cmake
│   ├── dependencies.cmake
│   ├── docs.cmake
│   ├── linker.cmake
│   ├── options.cmake
│   ├── packages.cmake
│   ├── section.cmake
│   └── summary.cmake
├── CMakeLists.txt
├── docs
│   ├── CMakeLists.txt
│   └── README.md.in
├── perf
│   └── CMakeLists.txt
├── README.md
├── src
│   ├── app.hpp
│   ├── camera.hpp
│   ├── CMakeLists.txt
│   ├── event.hpp
│   ├── filesystem.hpp
│   ├── glm.hpp
│   ├── graphics.hpp
│   ├── input.hpp
│   ├── layer.hpp
│   ├── layer_imgui.hpp
│   ├── log.hpp
│   ├── math.hpp
│   ├── perf.hpp
│   ├── platform
│   │   ├── CMakeLists.txt
│   │   └── linux
│   │       ├── CMakeLists.txt
│   │       ├── glfw.hpp
│   │       ├── graphics.hpp
│   │       ├── input.hpp
│   │       ├── sage.hpp
│   │       └── window.hpp
│   ├── repr.hpp
│   ├── sage.hpp
│   ├── std.cpp
│   ├── std.hpp
│   ├── time.hpp
│   ├── util.hpp
│   └── window.hpp
├── test
│   ├── CMakeLists.txt
│   ├── event.test.cpp
│   ├── glm.test.cpp
│   ├── layer_imgui.test.cpp
│   ├── layer.test.cpp
│   ├── linux_input.test.cpp
│   ├── linux_window.test.cpp
│   ├── log.test.cpp
│   ├── placeholder_layers.hpp
│   ├── test.hpp
│   └── util.test.cpp
└── TODO.md

12 directories, 67 files

```

