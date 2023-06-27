#include "repr.hpp"

struct Dump_Layer {
	size_t id;

	Dump_Layer(const size_t _id)
		: id{_id}
	{}

	auto setup() -> void { MESSAGE("DUMP Setting up ", id); }
	auto update() -> void { MESSAGE("DUMP Updating ", id); }
	auto teardown() -> void { MESSAGE("DUMP Tearing down ", id); }
	auto event_callback(const auto& event) -> void { MESSAGE("DUMP ", id, "got Event ", event); }

	REPR_DECL(Dump_Layer);
};

template <>
FMT_FORMATTER(Dump_Layer) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(Dump_Layer) {
		return fmt::format_to(ctx.out(), "Dump_Layer: {}", obj.id);
	}
};

REPR_DEF_FMT(Dump_Layer);

struct Other_Layer {
	size_t id;

	Other_Layer(const size_t _id)
		: id{_id}
	{}

	auto setup() -> void { MESSAGE("OTHER Setting up ", id); }
	auto update() -> void { MESSAGE("OTHER Updating ", id); }
	auto teardown() -> void { MESSAGE("OTHER Tearing down ", id); }
	auto event_callback(const auto& event) -> void { MESSAGE("OTHER Layer \"", id, "\" got Event ", event); }

	REPR_DECL(Other_Layer);
};

template <>
FMT_FORMATTER(Other_Layer) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(Other_Layer) {
		return fmt::format_to(ctx.out(), "Other_Layer: {}", obj.id);
	}
};

REPR_DEF_FMT(Other_Layer);

struct Last_Layer {
	size_t id;

	Last_Layer(const size_t _id)
		: id{_id}
	{}

	auto setup() -> void { MESSAGE("LAST Setting up ", id); }
	auto update() -> void { MESSAGE("LAST Updating ", id); }
	auto teardown() -> void { MESSAGE("LAST Tearing down ", id); }
	auto event_callback(const auto& event) -> void { MESSAGE("LAST Layer \"", id, "\" got Event ", event); }

	REPR_DECL(Last_Layer);
};

template <>
FMT_FORMATTER(Last_Layer) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(Last_Layer) {
		return fmt::format_to(ctx.out(), "Last_Layer: {}", obj.id);
	}
};

REPR_DEF_FMT(Last_Layer);
