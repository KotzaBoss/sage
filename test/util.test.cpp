#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "std.hpp"
#include "util.hpp"

using namespace sage;

TEST_CASE ("Trim") {
	auto input = std::string{},
		 result = std::string{};
	auto to_trim = ' ';

	SUBCASE ("Empty") {
		input = ""s;
		result = ""s;
	}

	SUBCASE ("Trimable spaces only left") {
		input = "            Test 123!"s;
		result = "Test 123!";
	}

	SUBCASE ("Trimable spaces only right") {
		input = "Test 123!            "s;
		result = "Test 123!";
	}

	SUBCASE ("Trimable spaces both sides") {
		input = "                    Test 123!            "s;
		result = "Test 123!";
	}

	SUBCASE ("No trimable spaces") {
		input = "Test 123!"s;
		result = "Test 123!";
	}

	SUBCASE ("Trim other character") {
		input = "!!!!!!!!!!!Test 0!!!!!!!!!!!!!!!!!!!!!!!";
		result = "Test 0";
		to_trim = '!';
	}

	string::trim(input, to_trim);
	CAPTURE(std::quoted(input));
	CAPTURE(std::quoted(result));
	CHECK_EQ(result, input);
}

TEST_CASE ("Type") {
	#pragma message "TODO: Weird behaviour for Set<>::count<>()"
	//CHECK_EQ(type::Set<>					::template count<>(), std::tuple{});
	CHECK_EQ(type::Set<>					::template count<int>(),				std::tuple{});
	CHECK_EQ(type::Set<int>					::template count<int>(),				std::make_tuple(1u));
	CHECK_EQ(type::Set<int, float>			::template count<float>(),				std::make_tuple(0u, 1u));
	CHECK_EQ(type::Set<int, float>			::template count<float, int, float>(),	std::make_tuple(1u, 2u));

	CHECK_EQ(type::Set<>					::template contains<int>(), false);
	CHECK_EQ(type::Set<>					::template contains<>(),	false);
	CHECK_EQ(type::Set<int>					::template contains<int>(), true);
	CHECK_EQ(type::Set<int, float>			::template contains<int>(), true);
	CHECK_EQ(type::Set<std::string, float>	::template contains<int>(), false);
}

TEST_CASE ("Polymorphic_Storage") {
	using Storage = util::Polymorphic_Storage<int, float, std::string>;

	const auto storage = Storage{
		1,2,3,4,5,
		"1"s, "2"s,
		1.f, 2.f, 3.f
	};

	storage.const_apply([] <typename T> (const Storage::Vector<T>& vec) {
			const auto capacity = vec.capacity(),
					   size = vec.size();
			CHECK_EQ(capacity, size);
			if constexpr (std::same_as<int, T>)
				CHECK_EQ(size, 5);
			else if constexpr (std::same_as<float, T>)
				CHECK_EQ(size, 3);
			else
				CHECK_EQ(size, 2);
		});
}
