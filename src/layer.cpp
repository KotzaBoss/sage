#include "layer.hpp"

namespace sage::layer {

						// Layer //

Layer::Layer(Args&& args)
	: _setup{std::move(args.setup)}
	, _update{std::move(args.update)}
	, _teardown{std::move(args.teardown)}
	, _event_callback{std::move(args.event_callback)}
{}

auto Layer::setup() -> void {
	std::invoke(_setup);
}

auto Layer::update() -> void {
	std::invoke(_update);
}

auto Layer::teardown() -> void {
	std::invoke(_teardown);
}

auto Layer::event_callback(const Event& e) -> void {
	std::invoke(_event_callback, e);
}

						// Layersss //

auto Layers::setup()	-> void { rg::for_each(stack, [] (auto& layer) { layer->setup();	}); }
auto Layers::update()	-> void { rg::for_each(stack, [] (auto& layer) { layer->update();	}); }
auto Layers::teardown() -> void { rg::for_each(stack, [] (auto& layer) { layer->teardown();	}); }

auto Layers::event_callback(const Event& e) -> void { rg::for_each(stack, [&] (auto& layer) { layer->event_callback(e); }); }

}// sage::layer
