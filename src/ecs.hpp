#pragma once

#include "src/std.hpp"

#include "src/math.hpp"
#include "src/util.hpp"

namespace sage::inline ecs {

namespace entity {

using Rep = size_t;

enum class Entity : Rep {};

constexpr auto rep(const Entity& e) -> Rep {
	return Rep(e);
}

constexpr auto null = Entity{std::numeric_limits<Rep>::max()};

}// ecs::entity


namespace component {

template <typename C>
concept Concept = std::semiregular<C>;	// copyable, movable, default_initializable

struct Transform {
	glm::mat4 trans = math::identity<glm::mat4>;
};

struct Sprite {
	glm::vec4 color = math::identity<glm::vec4>;
};

}// sage::ecs::components

template <component::Concept... Components>
struct ECS {
	using Entity = entity::Entity;
	using Entities = std::vector<std::optional<Entity>>;
	using Component_Storage = util::Polymorphic_Storage<std::optional<Components>...>;

private:
	Entities entities;
	Component_Storage components;

public:
	ECS(const size_t max_entities)
		: entities{max_entities}
		, components{max_entities}
	{}

public:
	[[nodiscard]]
	auto create() -> std::optional<Entity> {
		// OPTIMIZE: If necessary use a more sophisticated method
		//
		// A simple find(entities, std::nullopt) gives gcc a seizure...
		if (const auto entity = rg::find(entities, Entities::value_type{});
			entity != entities.end())
		{
			SAGE_ASSERT(not is_valid(*entity));

			const auto idx = static_cast<size_t>(std::distance(entities.begin(), entity));	// Ok to cast, it will be between begin/end
			components.apply_group([&] (const auto& comps) {
					SAGE_ASSERT(idx < comps.size(), "Components must be allocated");
					SAGE_ASSERT(not comps[idx].has_value(), "Cleanup has not been performed since last destroy/initialization");
				});

			*entity = Entity{idx};

			return *entity;
		}
		else
			return std::nullopt;
	}

	auto destroy(const Entity e) -> bool {
		const auto idx = entity::rep(e);
		if (not is_valid(idx))
			return false;
		else {
			entities[idx] = std::nullopt;
			components.apply_group([&] (auto& comps) {
					SAGE_ASSERT(idx < comps.size(), "Memory for components has not been allocated");
					rg::fill(comps, std::nullopt);
				});
			return true;
		}
	}

	// The Cs arguments can be deduced so prefer to call it without explicit template parameters:
	//
	// ecs.push(entt, Component1{...}, Component2{...});
	//
	template <typename... Cs>
		requires (sizeof...(Cs) > 0) and (type::Any<Cs, Components...> and ...) and type::Unique<Cs...>
	auto set_components(const Entity e, Cs&&... cs) -> decltype(auto /* optional<tuple<std::optional<Cs>&...>> */) {
		const auto idx = entity::rep(e);
		components.apply_group([&] (const auto& vec) {
				SAGE_ASSERT(idx < vec.size(), "Expect memory for components to be allocated");
			});

		auto comps = std::forward_as_tuple(
				std::get<typename Component_Storage::Vector<std::optional<Cs>>>(components)[idx] = std::forward<Cs>(cs)
				...
			);
		using Optional = std::optional<decltype(comps)>;

		return is_valid(idx)
			? Optional{comps}
			: Optional{std::nullopt}
			;
	}

	//
	template <typename... Cs>
		requires (sizeof...(Cs) > 0) and (type::Any<Cs, Components...> and ...) and type::Unique<Cs...>
	auto set_components(const Entity e, std::invocable<std::tuple<std::optional<Cs>&...>> auto&& fn) -> decltype(auto /* optional<tuple<std::optional<Cs>&...>> */) {
		const auto idx = entity::rep(e);
		components.apply_group([&] (const auto& vec) {
				SAGE_ASSERT(idx < vec.size(), "Expect memory for components to be allocated");
			});

		auto comps = std::forward_as_tuple(
				std::get<typename Component_Storage::Vector<std::optional<Cs>>>(components)[idx]
				...
			);
		using Optional = std::optional<decltype(comps)>;

		if (is_valid(idx)) {
			fn(comps);
			return Optional{comps};
		}
		else
			Optional{std::nullopt};
	}


	template <typename... Cs>
		requires (type::Any<Cs, Components...> and ...) and type::Unique<Cs...>
	auto components_of(const Entity e) -> decltype(auto /* optional<tuple<optional<Cs>&...>> */) {
		const auto idx = entity::rep(e);
		components.apply_group([&] (const auto& vec) {
				SAGE_ASSERT(idx < vec.size(), "Expect memory for components to be allocated");
			});

		auto comps = std::invoke([&] {
				if constexpr (sizeof...(Cs) == 0)
					// Fetch all Components
					return std::forward_as_tuple(
						std::get<typename Component_Storage::Vector<std::optional<Components>>>(components)[idx]
						...
					);
				else
					// Fetch only requested Components
					return std::forward_as_tuple(
						std::get<typename Component_Storage::Vector<std::optional<Cs>>>(components)[idx]
						...
					);
			});
		using Optional = std::optional<decltype(comps)>;

		return is_valid(idx)
			? Optional{std::move(comps)}
			: Optional{std::nullopt}
			;
	}

	template <typename... Cs>
		requires (sizeof...(Cs) > 0) and (type::Any<Cs, Components...> and ...) and type::Unique<Cs...>
	auto has_components(const Entity e) -> decltype(auto /* optional<tuple<bool...>> */) {
		const auto idx = entity::rep(e);
		components.apply_group([&] (const auto& vec) {
				SAGE_ASSERT(idx < vec.size(), "Expect memory for components to be allocated");
			});

		auto comps = std::make_tuple(
				std::get<typename Component_Storage::Vector<std::optional<Cs>>>(components)[idx].has_value()
				...
			);
		using Optional = std::optional<decltype(comps)>;

		return is_valid(idx)
			? Optional{std::move(comps)}
			: Optional{std::nullopt}
			;
	}

	// Return a view of tuples over all the valid entities and their components.
	// The first value of the tuples is an optional<Entity> that will always have a value.
	// The components are returned as optional<Component> and may/may not have value.
	//
	// CAUTION:
	// Should not be used as const.
	//
	// Do not:
	//
	// const auto view = ecs.view();
	// std::apply([] (const auto&... components) { ... });
	//
	// The compiler error says that the constraint for ranges::begin/end(__t)
	// is not satisfied but the explanation is propably:
	// https://stackoverflow.com/questions/66366084/why-cant-i-call-rangesbegin-on-a-const-filter-view
	//
	template <typename... Cs>
		requires (sizeof...(Cs) > 0) and (type::Any<Cs, Components...> and ...) and type::Unique<Cs...>
	auto view() -> decltype(auto /* view<tuple<optional<Entity>, optional<Cs>...>> */) {
		components.apply_group([&] (const auto& vec) {
				SAGE_ASSERT(vec.size() == entities.size(), "Entities and components must all have the same size");
			});

		return vw::zip(entities, std::get<typename Component_Storage::Vector<std::optional<Cs>>>(components)...)
			| vw::filter([&] (const auto& entt_comps) {
					return is_valid(std::get<0>(entt_comps));
				})
			;
	}

	template<type::Any<Entity, std::optional<Entity>, entity::Rep> Entt>
	auto is_valid(const Entt& e) const -> bool {
		const auto& entity = std::invoke([&] {
				if constexpr (std::same_as<Entity, Entt>)
					return entities[entity::rep(e)];
				else if constexpr (std::same_as<std::optional<Entity>, Entt>)
					return e;
				else if constexpr (std::same_as<entity::Rep, Entt>)
					return entities[e];
				else
					static_assert(false);
			});

		return entity.has_value();
	}


	auto size() const -> size_t {
		return rg::count_if(entities, [] (const auto& e) { return e.has_value(); });
	}

	auto clear() -> void {
		rg::fill(entities, std::nullopt);
	}
};

} //sage

template <>
FMT_FORMATTER(sage::entity::Entity) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::entity::Entity) {
		return fmt::format_to(ctx.out(), "Entity: {}", sage::entity::rep(obj));
	}
};

#ifdef SAGE_TEST_ECS
namespace {

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
		auto& ph =std::get<std::optional<Physics>&>(*comps);
		REQUIRE(ph.has_value());
		CHECK_EQ(ph->velocity, glm::vec2{e, e});
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

}
#endif
