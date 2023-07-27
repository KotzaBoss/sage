#pragma once

#include "std.hpp"

#include "log.hpp"

namespace sage::inline filesystem {

inline auto read_file(const fs::path& p) -> std::string {
	SAGE_ASSERT_PATH_READABLE(p);

	auto stream = std::ifstream{p, std::ios::in | std::ios::binary};
	SAGE_ASSERT(stream);

	auto content = std::string{};
	content.resize(fs::file_size(p));
	stream.read(content.data(), content.size());
	return content;
}

}// sage::filesystem
