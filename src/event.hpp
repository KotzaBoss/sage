#pragma once

#include "core.hpp"

namespace sage::inline event {

struct Event {
					// Type
	enum class Type {
		None = 0,
		Key_Pressed, Key_Released,
	} type;
	constexpr static inline auto bits_of_Type = bits<std::underlying_type_t<Type>>();

					// Category
	enum class Category : uint8_t {
		None = 0,
		Application	= 1 << 0,
		Input		= 1 << 1,
		Keyboard	= 1 << 2,
		Mouse		= 1 << 3
	} category;
	constexpr static inline auto bits_of_Category = bits<std::underlying_type_t<Category>>();

					// Payload
	using Payload = std::variant<int>;
	Payload payload;

public:
	friend auto operator<< (std::ostream& o, const Event& e) -> std::ostream& {
		return o
			<< "Event: "
			<< "Type:" << bits_of_Type << '=' << std::to_underlying(e.type) << ' '
			<< "Category:" << bits_of_Category << '=' << std::bitset<bits_of_Category>{std::to_underlying(e.category)}
			;
	}
};

struct Event_Dispatcher {
	// No clue
};

}// sage::inline
