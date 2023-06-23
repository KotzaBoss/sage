#pragma once

#include "std.hpp"

#include "core.hpp"
#include "math.hpp"
#include "log.hpp"

namespace sage::inline event {

struct Event {
					// Type
	enum class Type {
		None = 0,
		Window_Closed, Window_Resized,
	};
	constexpr static inline auto bits_of_Type = bits<std::underlying_type_t<Type>>();

					// Category
	// CAUTION: Do not forget to update the operator<< if the enum changes
	enum class Category : uint8_t {
		None = 0,
		Application	= 1 << 0,
		Input		= 1 << 1,
		Keyboard	= 1 << 2,
		Mouse		= 1 << 3
	};
	constexpr static inline auto bits_of_Category = bits<std::underlying_type_t<Category>>();

					// Payloads
	using Payload = std::variant<
		std::monostate,	// No payload
		Size<size_t>
	>;
	static constexpr auto no_payload = Payload{};

	using Callback = std::function<void(const Event&)>;
	static constexpr auto uninitialized_callback = [] (const Event&) { SAGE_ASSERT_MSG(false, "Event::Callback is uninitialized"); };

public:
	Type type;
	Category category;
	Payload payload;

public:
	static auto make_window_closed	()							-> Event;
	static auto make_window_resized	(const Size<size_t>& sz)	-> Event;

public:
	friend auto operator<< (std::ostream& o, const Category& c) -> std::ostream&;
	friend auto operator<< (std::ostream& o, const Event& e) -> std::ostream&;
};

struct Event_Dispatcher {
	// No clue
};

}// sage::inline
