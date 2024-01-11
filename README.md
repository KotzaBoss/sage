<!--  [toc] At some point it will be supported by Obsidian  -->

# SAGE: *Super Advanced Game Engine*

## Getting Started
```
git clone git@github.com:KotzaBoss/sage.git
cd sage
cmake -B build
cmake --build build -- sage
./build/bin/sage
```
To customize the build see [README#Build](README.md#build).

Once you have a successful build you can begin reading [bin/main.cpp](bin/main.cpp) and the tests to get a feel for the project.

## Development

> [!TIP]
> `cmake --build build -- usage`

A big part of SAGE is automated. Once you are done tweaking/extending/refactoring, make sure you _configure_ cmake at least once to generate docs.

### Build
```
cmake -B build	\
      -G Ninja	\
      -D CMAKE_LINKER=mold
cmake --build build
./build/bin/sage
```

| Option | Doc |
|:-|:-|
|`SAGE_OPT_CCACHE_ENABLED`| Use [`ccache`](https://ccache.dev/) to speedup subsequent compilations|
|`SAGE_OPT_OBSIDIAN`| Tell SAGE to use Gkiwnis' [.obsidian](https://obsidian.md/)|
|`SAGE_OPT_VERBOSE`| |

## Project Overview
14M used in 16 directories, 311 files.
```
./
├── asset/
│   ├── shader/
│   │   ├── CMakeLists.txt
│   │   ├── flat_color.glsl
│   │   ├── some.glsl
│   │   ├── square.glsl
│   │   └── texture.glsl
│   ├── texture/
│   │   ├── kenney_rpg-base/
│   │   │   ├── PNG/
│   │   │   │   ├── rpgTile000.png
│   │   │   │   ├── rpgTile001.png
│   │   │   │   ├── rpgTile002.png
│   │   │   │   ├── rpgTile003.png
│   │   │   │   ├── rpgTile004.png
│   │   │   │   ├── rpgTile005.png
│   │   │   │   ├── rpgTile006.png
│   │   │   │   ├── rpgTile007.png
│   │   │   │   ├── rpgTile008.png
│   │   │   │   ├── rpgTile009.png
│   │   │   │   ├── rpgTile010.png
│   │   │   │   ├── rpgTile011.png
│   │   │   │   ├── rpgTile012.png
│   │   │   │   ├── rpgTile013.png
│   │   │   │   ├── rpgTile014.png
│   │   │   │   ├── rpgTile015.png
│   │   │   │   ├── rpgTile016.png
│   │   │   │   ├── rpgTile017.png
│   │   │   │   ├── rpgTile018.png
│   │   │   │   ├── rpgTile019.png
│   │   │   │   ├── rpgTile020.png
│   │   │   │   ├── rpgTile021.png
│   │   │   │   ├── rpgTile022.png
│   │   │   │   ├── rpgTile023.png
│   │   │   │   ├── rpgTile024.png
│   │   │   │   ├── rpgTile025.png
│   │   │   │   ├── rpgTile026.png
│   │   │   │   ├── rpgTile027.png
│   │   │   │   ├── rpgTile028.png
│   │   │   │   ├── rpgTile029.png
│   │   │   │   ├── rpgTile030.png
│   │   │   │   ├── rpgTile031.png
│   │   │   │   ├── rpgTile032.png
│   │   │   │   ├── rpgTile033.png
│   │   │   │   ├── rpgTile034.png
│   │   │   │   ├── rpgTile035.png
│   │   │   │   ├── rpgTile036.png
│   │   │   │   ├── rpgTile037.png
│   │   │   │   ├── rpgTile038.png
│   │   │   │   ├── rpgTile039.png
│   │   │   │   ├── rpgTile040.png
│   │   │   │   ├── rpgTile041.png
│   │   │   │   ├── rpgTile042.png
│   │   │   │   ├── rpgTile043.png
│   │   │   │   ├── rpgTile044.png
│   │   │   │   ├── rpgTile045.png
│   │   │   │   ├── rpgTile046.png
│   │   │   │   ├── rpgTile047.png
│   │   │   │   ├── rpgTile048.png
│   │   │   │   ├── rpgTile049.png
│   │   │   │   ├── rpgTile050.png
│   │   │   │   ├── rpgTile051.png
│   │   │   │   ├── rpgTile052.png
│   │   │   │   ├── rpgTile053.png
│   │   │   │   ├── rpgTile054.png
│   │   │   │   ├── rpgTile055.png
│   │   │   │   ├── rpgTile056.png
│   │   │   │   ├── rpgTile057.png
│   │   │   │   ├── rpgTile058.png
│   │   │   │   ├── rpgTile059.png
│   │   │   │   ├── rpgTile060.png
│   │   │   │   ├── rpgTile061.png
│   │   │   │   ├── rpgTile062.png
│   │   │   │   ├── rpgTile063.png
│   │   │   │   ├── rpgTile064.png
│   │   │   │   ├── rpgTile065.png
│   │   │   │   ├── rpgTile066.png
│   │   │   │   ├── rpgTile067.png
│   │   │   │   ├── rpgTile068.png
│   │   │   │   ├── rpgTile069.png
│   │   │   │   ├── rpgTile070.png
│   │   │   │   ├── rpgTile071.png
│   │   │   │   ├── rpgTile072.png
│   │   │   │   ├── rpgTile073.png
│   │   │   │   ├── rpgTile074.png
│   │   │   │   ├── rpgTile075.png
│   │   │   │   ├── rpgTile076.png
│   │   │   │   ├── rpgTile077.png
│   │   │   │   ├── rpgTile078.png
│   │   │   │   ├── rpgTile079.png
│   │   │   │   ├── rpgTile080.png
│   │   │   │   ├── rpgTile081.png
│   │   │   │   ├── rpgTile082.png
│   │   │   │   ├── rpgTile083.png
│   │   │   │   ├── rpgTile084.png
│   │   │   │   ├── rpgTile085.png
│   │   │   │   ├── rpgTile086.png
│   │   │   │   ├── rpgTile087.png
│   │   │   │   ├── rpgTile088.png
│   │   │   │   ├── rpgTile089.png
│   │   │   │   ├── rpgTile090.png
│   │   │   │   ├── rpgTile091.png
│   │   │   │   ├── rpgTile092.png
│   │   │   │   ├── rpgTile093.png
│   │   │   │   ├── rpgTile094.png
│   │   │   │   ├── rpgTile095.png
│   │   │   │   ├── rpgTile096.png
│   │   │   │   ├── rpgTile097.png
│   │   │   │   ├── rpgTile098.png
│   │   │   │   ├── rpgTile099.png
│   │   │   │   ├── rpgTile100.png
│   │   │   │   ├── rpgTile101.png
│   │   │   │   ├── rpgTile102.png
│   │   │   │   ├── rpgTile103.png
│   │   │   │   ├── rpgTile104.png
│   │   │   │   ├── rpgTile105.png
│   │   │   │   ├── rpgTile106.png
│   │   │   │   ├── rpgTile107.png
│   │   │   │   ├── rpgTile108.png
│   │   │   │   ├── rpgTile109.png
│   │   │   │   ├── rpgTile110.png
│   │   │   │   ├── rpgTile111.png
│   │   │   │   ├── rpgTile112.png
│   │   │   │   ├── rpgTile113.png
│   │   │   │   ├── rpgTile114.png
│   │   │   │   ├── rpgTile115.png
│   │   │   │   ├── rpgTile116.png
│   │   │   │   ├── rpgTile117.png
│   │   │   │   ├── rpgTile118.png
│   │   │   │   ├── rpgTile119.png
│   │   │   │   ├── rpgTile120.png
│   │   │   │   ├── rpgTile121.png
│   │   │   │   ├── rpgTile122.png
│   │   │   │   ├── rpgTile123.png
│   │   │   │   ├── rpgTile124.png
│   │   │   │   ├── rpgTile125.png
│   │   │   │   ├── rpgTile126.png
│   │   │   │   ├── rpgTile127.png
│   │   │   │   ├── rpgTile128.png
│   │   │   │   ├── rpgTile129.png
│   │   │   │   ├── rpgTile130.png
│   │   │   │   ├── rpgTile131.png
│   │   │   │   ├── rpgTile132.png
│   │   │   │   ├── rpgTile133.png
│   │   │   │   ├── rpgTile134.png
│   │   │   │   ├── rpgTile135.png
│   │   │   │   ├── rpgTile136.png
│   │   │   │   ├── rpgTile137.png
│   │   │   │   ├── rpgTile138.png
│   │   │   │   ├── rpgTile139.png
│   │   │   │   ├── rpgTile140.png
│   │   │   │   ├── rpgTile141.png
│   │   │   │   ├── rpgTile142.png
│   │   │   │   ├── rpgTile143.png
│   │   │   │   ├── rpgTile144.png
│   │   │   │   ├── rpgTile145.png
│   │   │   │   ├── rpgTile146.png
│   │   │   │   ├── rpgTile147.png
│   │   │   │   ├── rpgTile148.png
│   │   │   │   ├── rpgTile149.png
│   │   │   │   ├── rpgTile150.png
│   │   │   │   ├── rpgTile151.png
│   │   │   │   ├── rpgTile152.png
│   │   │   │   ├── rpgTile153.png
│   │   │   │   ├── rpgTile154.png
│   │   │   │   ├── rpgTile155.png
│   │   │   │   ├── rpgTile156.png
│   │   │   │   ├── rpgTile157.png
│   │   │   │   ├── rpgTile158.png
│   │   │   │   ├── rpgTile159.png
│   │   │   │   ├── rpgTile160.png
│   │   │   │   ├── rpgTile161.png
│   │   │   │   ├── rpgTile162.png
│   │   │   │   ├── rpgTile163.png
│   │   │   │   ├── rpgTile164.png
│   │   │   │   ├── rpgTile165.png
│   │   │   │   ├── rpgTile166.png
│   │   │   │   ├── rpgTile167.png
│   │   │   │   ├── rpgTile168.png
│   │   │   │   ├── rpgTile169.png
│   │   │   │   ├── rpgTile170.png
│   │   │   │   ├── rpgTile171.png
│   │   │   │   ├── rpgTile172.png
│   │   │   │   ├── rpgTile173.png
│   │   │   │   ├── rpgTile174.png
│   │   │   │   ├── rpgTile175.png
│   │   │   │   ├── rpgTile176.png
│   │   │   │   ├── rpgTile177.png
│   │   │   │   ├── rpgTile178.png
│   │   │   │   ├── rpgTile179.png
│   │   │   │   ├── rpgTile180.png
│   │   │   │   ├── rpgTile181.png
│   │   │   │   ├── rpgTile182.png
│   │   │   │   ├── rpgTile183.png
│   │   │   │   ├── rpgTile184.png
│   │   │   │   ├── rpgTile185.png
│   │   │   │   ├── rpgTile186.png
│   │   │   │   ├── rpgTile187.png
│   │   │   │   ├── rpgTile188.png
│   │   │   │   ├── rpgTile189.png
│   │   │   │   ├── rpgTile190.png
│   │   │   │   ├── rpgTile191.png
│   │   │   │   ├── rpgTile192.png
│   │   │   │   ├── rpgTile193.png
│   │   │   │   ├── rpgTile194.png
│   │   │   │   ├── rpgTile195.png
│   │   │   │   ├── rpgTile196.png
│   │   │   │   ├── rpgTile197.png
│   │   │   │   ├── rpgTile198.png
│   │   │   │   ├── rpgTile199.png
│   │   │   │   ├── rpgTile200.png
│   │   │   │   ├── rpgTile201.png
│   │   │   │   ├── rpgTile202.png
│   │   │   │   ├── rpgTile203.png
│   │   │   │   ├── rpgTile204.png
│   │   │   │   ├── rpgTile205.png
│   │   │   │   ├── rpgTile206.png
│   │   │   │   ├── rpgTile207.png
│   │   │   │   ├── rpgTile208.png
│   │   │   │   ├── rpgTile209.png
│   │   │   │   ├── rpgTile210.png
│   │   │   │   ├── rpgTile211.png
│   │   │   │   ├── rpgTile212.png
│   │   │   │   ├── rpgTile213.png
│   │   │   │   ├── rpgTile214.png
│   │   │   │   ├── rpgTile215.png
│   │   │   │   ├── rpgTile216.png
│   │   │   │   ├── rpgTile217.png
│   │   │   │   ├── rpgTile218.png
│   │   │   │   ├── rpgTile219.png
│   │   │   │   ├── rpgTile220.png
│   │   │   │   ├── rpgTile221.png
│   │   │   │   ├── rpgTile222.png
│   │   │   │   ├── rpgTile223.png
│   │   │   │   ├── rpgTile224.png
│   │   │   │   ├── rpgTile225.png
│   │   │   │   ├── rpgTile226.png
│   │   │   │   ├── rpgTile227.png
│   │   │   │   └── rpgTile228.png
│   │   │   ├── Spritesheet/
│   │   │   │   ├── RPGpack_sheet_2X.png
│   │   │   │   └── RPGpack_sheet.png
│   │   │   ├── Vector/
│   │   │   │   ├── RPGpack_vector.svg
│   │   │   │   └── RPGpack_vector.swf
│   │   │   ├── license.txt
│   │   │   ├── preview.png
│   │   │   └── sample.png
│   │   ├── CMakeLists.txt
│   │   ├── owl.png
│   │   └── Ship.png
│   └── CMakeLists.txt
├── bin/
│   ├── CMakeLists.txt
│   ├── editor.hpp
│   ├── main.cpp
│   └── sandbox.hpp
├── cmake/
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
├── docs/
│   ├── CMakeLists.txt
│   ├── cpp_file.in
│   ├── README.md.in
│   ├── TODO.md.in
│   └── todo_note.md.in
├── perf/
│   └── CMakeLists.txt
├── src/
│   ├── platform/
│   │   ├── linux/
│   │   │   ├── CMakeLists.txt
│   │   │   ├── glfw.hpp
│   │   │   ├── graphics.hpp
│   │   │   ├── input.hpp
│   │   │   ├── sage.hpp
│   │   │   └── window.hpp
│   │   └── CMakeLists.txt
│   ├── app.hpp
│   ├── camera.hpp
│   ├── CMakeLists.txt
│   ├── ecs.hpp
│   ├── event.hpp
│   ├── filesystem.hpp
│   ├── fmt.hpp
│   ├── glm.hpp
│   ├── graphics.hpp
│   ├── input.hpp
│   ├── layer.hpp
│   ├── layer_imgui.hpp
│   ├── log.hpp
│   ├── math.hpp
│   ├── particle.hpp
│   ├── perf.hpp
│   ├── repr.hpp
│   ├── sage.hpp
│   ├── std.cpp
│   ├── std.hpp
│   ├── time.hpp
│   ├── util.hpp
│   └── window.hpp
├── test/
│   ├── CMakeLists.txt
│   ├── doctest.hpp
│   ├── placeholder_layers.hpp
│   ├── test.ecs.cpp
│   ├── test.event.cpp
│   ├── test.glm.cpp
│   ├── test.layer.cpp
│   ├── test.layer_imgui.cpp
│   ├── test.linux.input.cpp
│   ├── test.linux.window.cpp
│   ├── test.log.cpp
│   └── test.util.cpp
├── CMakeLists.txt
├── README.md
└── TODO.md
```

