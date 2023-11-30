# Overview

28 items to be done.

## TODO

21 items available.

| Source | Line Number | Description |
|:-|:-|:-|
| [test/util.test.cpp](test/util.test.cpp) | 64 | Weird behaviour for Set<>::count<>() |
| [src/util.hpp](src/util.hpp) | 331 | add check std>=23 todo deduce this |
| [src/util.hpp](src/util.hpp) | 329 | Deduce this to add const-correctness to Polymorphic_Containers. |
| [src/platform/linux/window.hpp](src/platform/linux/window.hpp) | 74 | Change switches to array lookups for clarity |
| [src/platform/linux/input.hpp](src/platform/linux/input.hpp) | 59 | Check error |
| [src/platform/linux/input.hpp](src/platform/linux/input.hpp) | 53 | Check error |
| [src/platform/linux/graphics.hpp](src/platform/linux/graphics.hpp) | 218 | This could be a range::chunk or range::slide in c++23? |
| [src/platform/linux/graphics.hpp](src/platform/linux/graphics.hpp) | 105 | Split dynamic/static vertex_buffer? |
| [src/layer_imgui.hpp](src/layer_imgui.hpp) | 131 | ImGui is abit special but is there a way to integrate this in .render()? |
| [src/layer.hpp](src/layer.hpp) | 15 | Maybe pass renderer::Concept to .render()? Can there be multiple renderers? |
| [src/layer.hpp](src/layer.hpp) | 14 | Maybe pass optional\<Event\> to allow for functional chaining? |
| [src/input.hpp](src/input.hpp) | 38 | OpenGL does not support mouse scroll polling, make the input Concept have a mouse_scroll() -> optional\<Mouse::Scroll\> ? |
| [src/graphics.hpp](src/graphics.hpp) | 472 | Use scene_active only in debug mode |
| [src/graphics.hpp](src/graphics.hpp) | 444 | More type safety |
| [src/graphics.hpp](src/graphics.hpp) | 424 | Query from GPU |
| [src/graphics.hpp](src/graphics.hpp) | 417 | Proper asset system and asset handles |
| [src/graphics.hpp](src/graphics.hpp) | 50 | Material system will differentiate set/upload_uniform? |
| [src/camera.hpp](src/camera.hpp) | 70 | Make a separete unit::Degrees struct? |
| [src/camera.hpp](src/camera.hpp) | 14 | Not sure if this API is necessary, maybe have public members? |
| [src/app.hpp](src/app.hpp) | 76 | Fixed rate updates how to? Hardcode to 144fps for now. |
| [bin/layer_2d.hpp](bin/layer_2d.hpp) | 11 | What to encapsulate, player.update(), player knows renderer? |

## FIXME

6 items available.

| Source | Line Number | Description |
|:-|:-|:-|
| [test/util.test.cpp](test/util.test.cpp) | 103 | Add constness, see pragma in Polymorphic_Array |
| [test/util.test.cpp](test/util.test.cpp) | 82 | Add constness, see pragma in Polymorphic_Array |
| [src/platform/linux/graphics.hpp](src/platform/linux/graphics.hpp) | 387 | why shader.size() doesnt work? its an array |
| [src/math.hpp](src/math.hpp) | 26 | default template does not work. |
| [src/graphics.hpp](src/graphics.hpp) | 22 | Is the uniform different from the shader::data::Types? |
| [cmake/docs.cmake](cmake/docs.cmake) | 102 | for platform/linux/sage.hpp it puts two `#linux` tags |

## CONFUSION

No items available.

## OPTIMIZE

1 items available.

| Source | Line Number | Description |
|:-|:-|:-|
| [src/platform/linux/graphics.hpp](src/platform/linux/graphics.hpp) | 592 | ? |

