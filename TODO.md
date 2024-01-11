# Overview

38 items to be done.

# Entries

## TODO

26 items available.

| Source | Line Number | Description |
|:-|:-|:-|
| [src/util.hpp](src/util.hpp) | 567 | Weird behaviour for Set<>::count<>() |
| [src/util.hpp](src/util.hpp) | 354 | add check std>=23 todo deduce this |
| [src/util.hpp](src/util.hpp) | 352 | Deduce this to add const-correctness to Polymorphic_Containers. |
| [src/platform/linux/window.hpp](src/platform/linux/window.hpp) | 74 | Change switches to array lookups for clarity |
| [src/platform/linux/input.hpp](src/platform/linux/input.hpp) | 61 | Check error |
| [src/platform/linux/input.hpp](src/platform/linux/input.hpp) | 55 | Check error |
| [src/platform/linux/graphics.hpp](src/platform/linux/graphics.hpp) | 690 | Query GPU |
| [src/platform/linux/graphics.hpp](src/platform/linux/graphics.hpp) | 312 | if constexpr (build::release) clear index/vertex buffers |
| [src/platform/linux/graphics.hpp](src/platform/linux/graphics.hpp) | 214 | This could be a range::chunk or range::slide in c++23? |
| [src/platform/linux/graphics.hpp](src/platform/linux/graphics.hpp) | 104 | Split dynamic/static vertex_buffer? |
| [src/perf.hpp](src/perf.hpp) | 233 | Make a specialization that is shorter than fmt's optional(...) |
| [src/layer_imgui.hpp](src/layer_imgui.hpp) | 129 | ImGui is abit special but is there a way to integrate this in .render()? |
| [src/input.hpp](src/input.hpp) | 38 | OpenGL does not support mouse scroll polling, make the input Concept have a mouse_scroll() -> optional\<Mouse::Scroll\> ? |
| [src/graphics.hpp](src/graphics.hpp) | 579 | Use scene_active only in debug mode |
| [src/graphics.hpp](src/graphics.hpp) | 497 | Make a namespace for shapes |
| [src/graphics.hpp](src/graphics.hpp) | 482 | More type safety |
| [src/graphics.hpp](src/graphics.hpp) | 461 | Query from GPU |
| [src/graphics.hpp](src/graphics.hpp) | 454 | Proper asset system and asset handles |
| [src/graphics.hpp](src/graphics.hpp) | 424 | Try to make this a constraint |
| [src/graphics.hpp](src/graphics.hpp) | 51 | Material system will differentiate set/upload_uniform? |
| [src/camera.hpp](src/camera.hpp) | 137 | Only use glm:: instead of the ad-hoc math::Size |
| [src/camera.hpp](src/camera.hpp) | 70 | Make a separete unit::Degrees struct? |
| [src/camera.hpp](src/camera.hpp) | 14 | Not sure if this API is necessary, maybe have public members? |
| [src/app.hpp](src/app.hpp) | 120 | Fixed rate updates how to? Hardcode to 144fps for now. |
| [bin/sandbox.hpp](bin/sandbox.hpp) | 11 | What to encapsulate, player.update(), player knows renderer? |
| [bin/editor.hpp](bin/editor.hpp) | 14 | How to "ignore" User_State |

## FIXME

8 items available.

| Source | Line Number | Description |
|:-|:-|:-|
| [src/util.hpp](src/util.hpp) | 629 | Add constness, see pragma in Polymorphic_Array |
| [src/util.hpp](src/util.hpp) | 590 | Add constness, see pragma in Polymorphic_Array |
| [src/platform/linux/graphics.hpp](src/platform/linux/graphics.hpp) | 385 | why shader.size() doesnt work? its an array |
| [src/particle.hpp](src/particle.hpp) | 44 | Why the default template for Lifetime doesnt work? |
| [src/math.hpp](src/math.hpp) | 40 | default template does not work. |
| [src/graphics.hpp](src/graphics.hpp) | 23 | Is the uniform different from the shader::data::Types? |
| [cmake/docs.cmake](cmake/docs.cmake) | 278 | Remove the extra newlines added at the end of the file |
| [cmake/docs.cmake](cmake/docs.cmake) | 102 | for platform/linux/sage.hpp it puts two `#linux` tags |

## CONFUSION

1 items available.

| Source | Line Number | Description |
|:-|:-|:-|
| [cmake/docs.cmake](cmake/docs.cmake) | 193 | There is some when it comes to portable cmake. Should we just use execute_process so that we do not |

## RND

1 items available.

| Source | Line Number | Description |
|:-|:-|:-|
| [src/particle.hpp](src/particle.hpp) | 61 | How would this parallel-array conditional transformation work cpu-cache wise? |

## OPTIMIZE

2 items available.

| Source | Line Number | Description |
|:-|:-|:-|
| [src/platform/linux/graphics.hpp](src/platform/linux/graphics.hpp) | 590 | ? |
| [src/ecs.hpp](src/ecs.hpp) | 59 | If necessary use a more sophisticated method |



