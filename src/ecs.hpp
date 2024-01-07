#pragma once

#include "src/std.hpp"

#include "src/math.hpp"
#include "src/util.hpp"

namespace sage::inline ecs {

namespace entity {

using Rep = size_t;

enum class Entity : size_t {};

constexpr auto rep(const Entity& e) -> Rep {
	return Rep(e);
}

constexpr auto null = Entity{std::numeric_limits<Rep>::max()};

}// ecs::entity


namespace component {

template <typename C>
concept Concept = std::semiregular<C>;

using Transform = glm::mat4;

}// sage::ecs::components

template <component::Concept... _Components>
struct ECS {
	using Entity = entity::Entity;
	using Entities = std::vector<std::optional<Entity>>;
	using Components = util::Polymorphic_Storage<_Components...>;

private:
	Entities entities;
	Components components;

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
			SAGE_ASSERT(not entity->has_value());

			const auto idx = std::distance(entities.begin(), entity);
			*entity = Entity{static_cast<size_t>(idx)};

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

			return true;
		}
	}

	// The Cs arguments can be deduced so prefer to call it without explicit template parameters:
	//
	// ecs.push(entt, Component1{...}, Component2{...} );
	//
	template <typename... Cs>
		requires (type::Any<Cs, _Components...> and ...) and type::Unique<Cs...>
	auto push(const Entity e, Cs&&... cs) -> bool {
		const auto idx = entity::rep(e);
		if (not is_valid(idx))
			return false;
		else {
			(
				std::invoke([&] {
					auto& vec = std::get<typename Components::Vector<Cs>>(components);
					vec[idx] = std::forward<Cs>(cs);
				})
				, ...
			);

			return true;
		}
	}

	// Should not be used as const.
	//
	// Do not:
	//
	// const auto view = ecs.view();
	// std::apply([] (const auto&... components) { ... };
	//
	// The compiler error says that the constraint for ranges::begin/end(__t)
	// is not satisfied but the explanation is propably:
	// https://stackoverflow.com/questions/66366084/why-cant-i-call-rangesbegin-on-a-const-filter-view
	//
	template <typename... Cs>
		requires (type::Any<Cs, _Components...> and ...) and type::Unique<Cs...>
	auto view() -> decltype(auto) /* view<tuple<optional<Entity>, Cs...>>*/ {
		components.apply_group([&] (const auto& vec) {
				SAGE_ASSERT(vec.size() == entities.size());
			});

		return vw::zip(entities, std::get<typename Components::Vector<Cs>>(components)...)
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
};

} //sage

template <>
FMT_FORMATTER(sage::entity::Entity) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::entity::Entity) {
		return fmt::format_to(ctx.out(), "Entity: {}", sage::entity::rep(obj));
	}
};

