#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "test/test.hpp"

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
	CHECK_EQ(type::Any<int>,				false);
	CHECK_EQ(type::Any<int, int>,			true);
	CHECK_EQ(type::Any<int, float, int>,	true);
	CHECK_EQ(type::Any<int, double>,		false);

	CHECK_EQ(type::Not_In<int>,				true);
	CHECK_EQ(type::Not_In<int, int>,		false);
	CHECK_EQ(type::Not_In<int, float, int>,	false);
	CHECK_EQ(type::Not_In<int, double>,		true);

	CHECK_EQ(type::Unique<>,				true);
	CHECK_EQ(type::Unique<int>,				true);
	CHECK_EQ(type::Unique<int, float, int>,	false);
	CHECK_EQ(type::Unique<int, double>,		true);

	// TODO: Weird behaviour for Set<>::count<>()
	//CHECK_EQ(type::Set<>					::template count<>(), std::tuple{});
	CHECK_EQ(type::Set<>					::template count<int>(),						std::tuple{});
	CHECK_EQ(type::Set<int>					::template count<int>(),						std::make_tuple(1u));
	CHECK_EQ(type::Set<int, float>			::template count<float>(),						std::make_tuple(0u, 1u));
	CHECK_EQ(type::Set<int, float>			::template count<float, int, float>(),			std::make_tuple(1u, 2u));
	CHECK_EQ(type::Set<int, float>			::template count<float, int, double, float>(),	std::make_tuple(1u, 2u));

	CHECK_EQ(type::Set<>					::template contains<int>(), false);
	CHECK_EQ(type::Set<>					::template contains<>(),	false);
	CHECK_EQ(type::Set<int>					::template contains<int>(), true);
	CHECK_EQ(type::Set<int, float>			::template contains<int>(), true);
	CHECK_EQ(type::Set<std::string, float>	::template contains<int>(), false);
}

TEST_CASE ("Polymorphic_Array") {
	using Array = util::Polymorphic_Array<int, float, std::string>;

	SUBCASE ("Default Construction") {
		auto storage = Array{};
	}

	SUBCASE ("Initialized") {
		// FIXME: Add constness, see pragma in Polymorphic_Array
		auto storage = Array{1, 2.0f, "3"s};
		auto order_flags = std::bitset<3>();

		CAPTURE(storage);

		storage.apply([&] (const auto& x) {
				using T = std::decay_t<decltype(x)>;
				if constexpr (std::same_as<T, int>) {
					REQUIRE_MESSAGE(order_flags.none(), "Implementation does not keep template order");
					CHECK_EQ(x, 1);
					order_flags.flip(0);
				}
				else if constexpr (std::same_as<T, float>) {
					REQUIRE_MESSAGE((order_flags.count() == 1 and order_flags[0]), "Implementation does not keep template order");
					CHECK_EQ(x, 2.0);
					order_flags.flip(1);
				}
				else if constexpr (std::same_as<T, std::string>) {
					REQUIRE_MESSAGE((order_flags.count() == 2 and order_flags[0] and order_flags[1]), "Implementation does not keep template order");
					CHECK_EQ(x, "3");
					order_flags.flip(2);
				}
				else
					FAIL("Unexpected type in Polymorphic_Array");
			});

		CHECK(order_flags.all());
	}
}

TEST_CASE ("Polymorphic_Storage") {
	using Storage = util::Polymorphic_Storage<int, float, std::string>;

	SUBCASE ("Default Construction") {
		auto storage = Storage{};
	}

	SUBCASE ("Initialized") {
		// FIXME: Add constness, see pragma in Polymorphic_Array
		auto storage = Storage{
			{1,2,3,4,5},
			{1.f, 2.f, 3.f},
			{"1"s, "2"s}
		};

		CAPTURE(storage);

		storage.apply_group([] (const auto& vec) {
				using T = typename std::decay_t<decltype(vec)>::value_type;
				const auto capacity = vec.capacity(),
						   size = vec.size();
				CHECK_EQ(capacity, size);
				if constexpr (std::same_as<int, T>)
					CHECK_EQ(size, 5);
				else if constexpr (std::same_as<float, T>)
					CHECK_EQ(size, 3);
				else if constexpr (std::same_as<T, std::string>)
					CHECK_EQ(size, 2);
				else
					FAIL("Unexpected type in Polymorphic_Array");
			});
	}
}

TEST_CASE ("toogle_if") {
	auto b = true;

	for (auto i = 0; i < 1000; ++i) {
		const auto cond = random::toggle::any();
		const auto prev = b;

		b = toogle_if(b, cond);
		MESSAGE(b);
		if (cond)
			REQUIRE_NE(b, prev);
		else
			REQUIRE_EQ(b, prev);
	}
}
