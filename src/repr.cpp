#include "formatter.hpp"
#include "log.hpp"
#include "repr.hpp"


#include "math.hpp"
namespace sage::math {

template<sage::Number N>
REPR_DEF_FMT(sage::Size<N>);

}


#include "event.hpp"
namespace sage::event {

REPR_DEF_FMT(Event::Type);
REPR_DEF_FMT(Event::Category);
REPR_DEF_FMT(Event::Payload);
REPR_DEF_FMT(Event);

}


#include "window.hpp"
namespace sage::window {
REPR_DEF_FMT(Properties)
}


#include "platform/linux/window.hpp"
namespace sage::oslinux::window {
REPR_DEF_FMT(Window)
}

