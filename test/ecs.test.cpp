#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "test/test.hpp"

using namespace sage;


TEST_CASE ("Must not compile") {
	//const Entity ent = 666;

	//const auto func = [] (const size_t e) {
	//	return e + 1;
	//};
	//func(ent);
}

constexpr auto infinity = std::numeric_limits<float>::infinity();

TEST_CASE ("ECS") {
	struct Physics {
		glm::vec2 velocity;
	};

	struct Collision {
		glm::vec2 a, b;
	};

	constexpr auto max_entities = 100ul;

	auto entities = std::vector<entity::Entity>{};
	entities.reserve(max_entities);
	constexpr auto test_entity = entity::Entity{0};

	auto ecs = ECS<Physics, Collision>{max_entities};

	// Create
	for (const auto e : vw::iota(0ul, max_entities)) {
		const auto entt = ecs.create();
		SAGE_ASSERT(entt.has_value());

		entities.push_back(*entt);

		auto comps = ecs.set_components(*entt, Physics{{e, e}}, Collision{{e, e}, {e, e}});
		CHECK(comps.has_value());
		CHECK_EQ(std::get<Physics&>(*comps).velocity, glm::vec2{e, e});
	}
	CHECK_EQ(ecs.size(), max_entities);

	// View
	auto view = ecs.view<Physics, Collision>();
	REQUIRE(not view.empty());
	rg::for_each(view, [&] (const auto& entt_comps) {
			const auto& [_e, ph, col] = entt_comps;
			SAGE_ASSERT(_e.has_value() and ph.has_value() and col.has_value());

			const auto e = entity::rep(*_e);
			CHECK_EQ(e, ph->velocity.x);
			CHECK_EQ(e, ph->velocity.y);
			CHECK_EQ(e, col->a.x);
			CHECK_EQ(e, col->a.y);
			CHECK_EQ(e, col->b.x);
			CHECK_EQ(e, col->b.y);

			// Tweak values of one to confirm they are retained
			if (_e == test_entity) {
				ph->velocity.x =
				ph->velocity.y =
				col->a.x =
				col->a.y =
				col->b.x =
				col->b.y = infinity
				;
			}
		});

	{
		const auto bools = ecs.has_components<Physics>(test_entity);
		REQUIRE(bools.has_value());
		CHECK_EQ(std::tuple_size_v<std::decay_t<decltype(*bools)>>, 1);
		CHECK(std::get<0>(*bools));
	}

	{
		const auto bools = ecs.has_components<Physics, Collision>(test_entity);
		REQUIRE(bools.has_value());
		CHECK_EQ(std::tuple_size_v<std::decay_t<decltype(*bools)>>, 2);
		std::apply([] (const auto&... b) { (CHECK(b), ...); }, *bools);
	}

	// Check if changes remain
	const auto components = ecs.components_of<Physics, Collision>(test_entity);
	REQUIRE(components.has_value());
	std::apply(
			[&] <typename... Cs> (const Cs&... comps) {
				CHECK_GT(sizeof...(comps), 0);
				(
					std::invoke([&] {
						REQUIRE(comps.has_value());
						if constexpr (std::same_as<std::optional<Physics>, Cs>) {
							CHECK_EQ(comps->velocity, glm::vec2{infinity, infinity});
						}
						else if constexpr (std::same_as<std::optional<Collision>, Cs>) {
							CHECK_EQ(comps->a, comps->b);
							CHECK_EQ(comps->a, glm::vec2{infinity, infinity});
						}
						else
							static_assert(false);
					})
					, ...
				);
			},
			*components
		);

	// Destroy
	CHECK(ecs.destroy(test_entity));
	CHECK_EQ(ecs.size(), max_entities - 1);

	// Clear
	ecs.clear();
	CHECK_EQ(ecs.size(), 0);
	CHECK(ecs.view<Physics, Collision>().empty());
}
