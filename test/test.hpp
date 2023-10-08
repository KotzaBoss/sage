#pragma once

#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
#include "doctest.h"

#include "sage.hpp"

#include "platform/linux/window.hpp"
#include "platform/linux/input.hpp"

namespace sage::event {
REPR_DEF_FMT(Event::Type);
REPR_DEF_FMT(Event::Category);
REPR_DEF_FMT(Event::Payload);
REPR_DEF_FMT(Event);
}

namespace sage::layer {
REPR_DEF_FMT(ImGui);
}


namespace sage::window {
REPR_DEF_FMT(Properties)
}


namespace sage::oslinux::window {
REPR_DEF_FMT(Window)
}

namespace sage::oslinux::graphics {
REPR_DEF_FMT(Vertex_Buffer)
}

namespace sage::input {
REPR_DEF_FMT(Mouse::Pos);
}

namespace sage::graphics::buffer {
REPR_DEF_FMT(Element);
}

