#include "event.hpp"

namespace sage::event {

auto Event::make_window_closed() -> Event {
	return {
		.type = Type::Window_Closed,
		.category = Category::Input,
		.payload = Event::no_payload
	};
}

auto Event::make_window_resized(Size<int>&& sz) -> Event {
	return {
		.type = Type::Window_Resized,
		.category = Category::Input,
		.payload = std::move(sz)
	};
}


auto operator<< (std::ostream& o, const Event::Category& c) -> std::ostream& {
	static constexpr auto category_repr = std::array{
		"Application",	// 0001 index 0
		"Input",	// 0010 index 1
		"Keyboard",	// 0100 index 2
		"Mouse",
		"None",		// 0000 index last to make it work
		};

	// I indeed am losing my mind . . .

	const auto cat_bitset = std::bitset<Event::bits_of_Category>{std::to_underlying(c)};
	SAGE_ASSERT(cat_bitset.count() <= 1);

	const auto reversed_cat_bitset = vw::reverse(cat_bitset.to_string());

	const auto iter = rg::find(reversed_cat_bitset, '1');
	const auto index = iter != reversed_cat_bitset.end()
		? std::distance(reversed_cat_bitset.begin(), iter)
		: reversed_cat_bitset.size() - 1  // last
		;
	SAGE_ASSERT_MSG((index < category_repr.size()), category_repr.size());

	const auto cat_repr = category_repr[index];

	return o
		<< "Category: " << Event::bits_of_Category << '=' << cat_bitset
		<< '(' << cat_repr << ");"
		;
}

auto operator<< (std::ostream& o, const Event& e) -> std::ostream& {
	o
		<< "Event: "
		<< "Type: " << Event::bits_of_Type << '=' << std::to_underlying(e.type) << "; "
		<< e.category << ' '
		;
	std::visit(
		[&] (auto&& payload) {
			o << "Payload: ";
			using T = std::decay_t<decltype(payload)>;
			if constexpr (std::same_as<T, std::monostate>)
				o << "monostate;";
			else
				o << payload;
		},
		e.payload
	);
	return o << ';';
}

}// sage::event
