#pragma once

#include <version>
#include <concepts>
#ifdef __cpp_lib_coroutine
	#include <coroutine>
#endif
#include <any>
#include <bitset>
#include <chrono>
#include <compare>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <initializer_list>
#include <optional>
#ifdef __cpp_lib_source_location
	#include <source_location>
#endif
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <variant>
#include <memory>
#ifdef __cpp_lib_memory_resource
	#include <memory_resource>
#endif
#include <new>
#include <scoped_allocator>
#include <cfloat>
#include <cinttypes>
#include <climits>
#include <cstdint>
#include <limits>
#include <cassert>
#include <cerrno>
#include <exception>
#include <stdexcept>
#include <system_error>
#include <cctype>
#include <charconv>
#include <cstring>
#if __has_include(<cuchar>)
	#include <cuchar>
#endif
#include <cwchar>
#include <cwctype>
// The library {fmt} shall be used
//#ifdef __cpp_lib_format
//	#include <format>
//#endif
#include <string>
#include <string_view>
#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <span>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iterator>
#include <ranges>
namespace rg = std::ranges;
namespace vw = std::views;
#include <algorithm>
#include <bit>
#include <cfenv>
#include <cmath>
#include <complex>
#include <numbers>
#include <numeric>
#include <random>
#include <ratio>
#include <valarray>
#include <clocale>
#include <codecvt>
#include <locale>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <ostream>
#ifdef __cpp_lib_spanstream
	#include <spanstream>
#endif
#include <sstream>
#include <streambuf>
#ifdef __cpp_lib_syncstream
	#include <syncstream>
#endif
#include <filesystem>
namespace fs = std::filesystem;
#include <regex>
#include <atomic>
#ifdef __cpp_lib_barrier
	#include <barrier>
#endif
#include <condition_variable>
#include <future>
#ifdef __cpp_lib_latch
	#include <latch>
#endif
#include <mutex>
#ifdef __cpp_lib_semaphore
	#include <semaphore>
#endif
#include <shared_mutex>
#ifdef __cpp_lib_jthread
	#include <stop_token>
#endif
#include <thread>
#include <iso646.h>
#ifdef __cpp_lib_expected
	#include <expected>
#endif

#include <sys/prctl.h>

#include <cxxabi.h>

using namespace std::literals;

namespace build {

#ifdef NDEBUG
#define SAGE_DEBUG false
#else
#define SAGE_DEBUG true
#endif

constexpr auto debug = SAGE_DEBUG;
constexpr auto release = not debug;

}// build
