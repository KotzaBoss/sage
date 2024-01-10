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

