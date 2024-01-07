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

	auto ecs = ECS<Physics, Collision>{max_entities};

	// Create
	for (const auto e : vw::iota(0ul, max_entities)) {
		const auto entt = ecs.create();
		SAGE_ASSERT(entt.has_value());

		entities.push_back(*entt);

		CHECK(ecs.push(*entt, Physics{{e, e}}, Collision{{e, e}, {e, e}}));
	}
	CHECK_EQ(ecs.size(), max_entities);

	// View
	auto view = ecs.view<Physics, Collision>();
	rg::for_each(view, [] (const auto& entt_comps) {
			const auto& [_e, ph, col] = entt_comps;
			SAGE_ASSERT(_e.has_value());

			const auto e = entity::rep(*_e);
			CHECK_EQ(e, ph.velocity.x);
			CHECK_EQ(e, ph.velocity.y);
			CHECK_EQ(e, col.a.x);
			CHECK_EQ(e, col.a.y);
			CHECK_EQ(e, col.b.x);
			CHECK_EQ(e, col.b.y);
		});

	// Destroy
	for (const auto& e : entities) {
		CHECK(ecs.destroy(e));
	}
	CHECK_EQ(ecs.size(), 0ul);
}
