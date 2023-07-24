#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "util.hpp"

using namespace sage;

TEST_CASE ("Event") {
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
