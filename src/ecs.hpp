#pragma once

#include "src/std.hpp"

#include "src/math.hpp"
#include "src/util.hpp"
#include "src/camera.hpp"

namespace sage::inline ecs {

namespace entity {

using ID = util::ID;

}// ecs::entity


namespace component {

template <typename C>
concept Concept =
		std::semiregular<C>	// copyable, movable, default_initializable
	and requires {
		{ C::type_name() } -> std::same_as<std::string_view>;
	}
	;

#define SAGE_ECS_TYPE_NAME_GETTER(klass)	\
	static constexpr auto type_name() -> std::string_view {	\
		return std::string_view{#klass};	\
	}

// TODO: The components need rework to be flexible. The problem is that if the templates
//       are kept then the Components... cant really be passed to App...

struct Name {
private:
	inline static auto counter = 1ul;

public:
	std::string name = fmt::format("Unamed Entity {}", counter++);

	SAGE_ECS_TYPE_NAME_GETTER(Name)
};

struct Transform {
	glm::mat4 trans = math::identity<glm::mat4>;

	SAGE_ECS_TYPE_NAME_GETTER(Transform)
};

struct Sprite {
	glm::vec4 color = math::identity<glm::vec4>;

	SAGE_ECS_TYPE_NAME_GETTER(Sprite)
};

struct Camera {
	camera::Scene_Camera camera;
	bool has_fixed_aspect_ratio = false;

	SAGE_ECS_TYPE_NAME_GETTER(Camera)
};

struct Position {
	glm::vec3 position;

	SAGE_ECS_TYPE_NAME_GETTER(Position)
};

#define _ALL_COMPONENTS \
	component::Name,	\
	component::Transform,	\
	component::Sprite,	\
	component::Camera,	\
	component::Position
	/* Add new component here with no comma at the end and dont forget the '\' at the end of the item above */

}// sage::ecs::components


// TODO: Rethink what is returned by components_of, view.
//       How should unset components be filtered? Atm the are optional but perhaps they should
//       be skipped entirely with a vw::filter?
template <component::Concept... Components>
struct Basic_ECS {
	using IDs = std::vector<entity::ID>;
	using Component_Storage = util::Polymorphic_Storage<std::optional<Components>...>;

	// Returned to the user but should only be constructed and assigned by ECS.
	struct Entity {
		friend struct Basic_ECS;	// Only ECS can tweak internals

	private:
		entity::ID _id;
		Basic_ECS* ecs;

	public:
		Entity()
			: ecs{nullptr}
		{}

		Entity(entity::ID id, Basic_ECS* _parent)
			: _id{std::move(id)}
			, ecs{_parent}
		{}

		Entity(Entity&& e)
			: _id{std::exchange(e._id, std::nullopt)}
			, ecs{std::exchange(e.ecs, nullptr)}
		{}

		auto operator= (Entity&& e) -> Entity& {
			SAGE_ASSERT(e.ecs != nullptr, "Attempting to assign from a null Entity");
			SAGE_ASSERT(ecs == nullptr or ecs == e.ecs, "Entities do not come from the same ECS");

			_id = std::exchange(e._id, std::nullopt);
			ecs = std::exchange(e.ecs, nullptr);
			return *this;
		}

						// See Basic_ECS for details
	public:
		template <typename... Cs>
		auto set(Cs&&... cs) -> decltype(auto) {
			SAGE_ASSERT(ecs != nullptr);
			return ecs->set_components<Cs...>(*this, std::forward<Cs>(cs)...);
		}

		template <typename... Cs>
		auto set(std::invocable<std::tuple<std::optional<Cs>&...>> auto&& fn) -> decltype(auto) {
			SAGE_ASSERT(ecs != nullptr);
			return ecs->set_components<Cs...>(std::forward<decltype(fn)>(fn));
		}

		template <typename... Cs>
		auto components() -> decltype(auto) {
			SAGE_ASSERT(ecs != nullptr);
			return ecs->components_of<Cs...>(*this);
		}

		template <typename... Cs>
		auto has() -> decltype(auto) {
			SAGE_ASSERT(ecs != nullptr);
			return ecs->has_components<Cs...>(*this);
		}

		auto is_valid() const -> bool {
			return ecs->is_valid(*this);
		}

		auto operator== (const Entity& e) const -> bool {
			return ecs != nullptr and e.ecs != nullptr and _id == e._id and ecs == e.ecs;
		}

		auto id() const -> const entity::ID& { return _id; }

	public:
		friend FMT_FORMATTER(Entity);
	};

private:
	IDs ids;
	Component_Storage components;


public:
	Basic_ECS(const size_t max_entities)
		: ids{max_entities}
		, components{max_entities}
	{}

public:
	[[nodiscard]]
	auto create() -> std::optional<Entity> {
		// OPTIMIZE: If necessary use a more sophisticated method
		//
		// A simple find(entities, std::nullopt) gives gcc a seizure...
		if (const auto id = rg::find(ids, IDs::value_type{});
			id != ids.end())
		{
			SAGE_ASSERT(not is_valid(*id));

			const auto idx = static_cast<entity::ID::Rep>(std::distance(ids.begin(), id));	// Ok to cast, it will be between begin/end
			components.apply_group([&] (const auto& comps) {
					SAGE_ASSERT(idx < comps.size(), "Components must be allocated");
					SAGE_ASSERT(not comps[idx].has_value(), "Cleanup has not been performed since last destroy/initialization");
				});

			*id = entity::ID{Raw_ID{idx}};

			return std::make_optional<Entity>(*id, this);	// Wont bother dealing with "cannot be converted from brace enclosed initializer list to Entity"...
		}
		else
			return std::nullopt;
	}

	auto null() -> Entity {
		return { std::nullopt, this };
	}

	auto destroy(Entity& e) -> bool {
		if (not is_valid(e))
			return false;
		else {
			const auto idx = e._id.raw();
			ids[idx] = std::nullopt;
			components.apply_group([&] (auto& comps) {
					SAGE_ASSERT(idx < comps.size(), "Memory for components has not been allocated");
					comps[idx] = std::nullopt;
				});
			e._id.reset();

			return true;
		}
	}

	// The Cs arguments can be deduced so prefer to call it without explicit template parameters:
	//
	// ecs.push(entt, Component1{...}, Component2{...});
	//
	template <typename... Cs>
		requires (sizeof...(Cs) > 0) and (type::Any<Cs, Components...> and ...) and type::Unique<Cs...>
	auto set_components(const Entity& e, Cs&&... cs) -> decltype(auto /* optional<tuple<std::optional<Cs>&...>> */) {
		using Optional = std::optional<std::tuple<std::optional<Cs>&...>>;

		if (not is_valid(e))
			return Optional{std::nullopt};
		else {
			const auto idx = e._id.raw();
			auto comps = std::forward_as_tuple(
					std::get<typename Component_Storage::Vector<std::optional<Cs>>>(components)[idx] = std::forward<Cs>(cs)
					...
				);
			return Optional{comps};
		}
	}

	template <typename... Cs>
		requires (type::Any<Cs, Components...> and ...) and type::Unique<Cs...>
	auto components_of(const Entity& e) -> decltype(auto /* optional<tuple<optional<Cs>&...>> */) {
		// Even if entity is not valid we can reference the first element to make the type deduction work
		const auto idx = std::to_underlying(e._id.value_or(Raw_ID{0}));
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

		return is_valid(e)
			? Optional{comps}
			: Optional{std::nullopt}
			;
	}

	template <typename... Cs>
		requires (sizeof...(Cs) > 0) and (type::Any<Cs, Components...> and ...) and type::Unique<Cs...>
	auto has_components(const Entity& e) -> decltype(auto /* optional<tuple<bool...>> */) {
		using Optional = std::optional<decltype(std::make_tuple(
				std::get<typename Component_Storage::Vector<std::optional<Cs>>>(components).front().has_value()
				...
			))>;

		if (not is_valid(e))
			return Optional{std::nullopt};
		else {
			const auto idx = e._id.raw();
			auto comps = std::make_tuple(
				std::get<typename Component_Storage::Vector<std::optional<Cs>>>(components)[idx].has_value()
				...
			);
			return Optional{comps};
		}
	}

	// Return a view of tuples over all the valid ids and their components.
	// The first value of the tuples is an entity::ID that will always have a value.
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
	// TODO: if sizeof...(Cs) == 0 return all
	template <typename... Cs>
		requires (sizeof...(Cs) > 0) and (type::Any<Cs, Components...> and ...) and type::Unique<Cs...>
	auto view() -> decltype(auto /* view<tuple<entity::ID, optional<Cs>...>> */) {
		components.apply_group([&] (const auto& vec) {
				SAGE_ASSERT(vec.size() == ids.size(), "IDs and components must all have the same size");
			});

		return vw::zip(ids, std::get<typename Component_Storage::Vector<std::optional<Cs>>>(components)...)
			| vw::filter([&] (auto&& entt_comps) {
					return is_valid(std::get<0>(entt_comps));
				})
			;
	}

	template <type::Any<Entity, entity::ID> Entt>
	auto is_valid(const Entt& e) const -> bool {
		if constexpr (std::same_as<Entt, Entity>)
			return e.ecs == this and e._id.has_value() and ids[e._id.raw()] == e._id;
		else if constexpr (std::same_as<Entt, entity::ID>)
			return e.has_value();
		else
			static_assert(false);
	}

	// ECS size is not the same as a vector size. ECS should preallocate its vectors.
	//
	auto size() /* const */ -> size_t {
		// TODO: Fix constness of Polymorphic_* containers
		//components.apply_group([&] (auto& vec) {
		//		SAGE_ASSERT(ids.size() == vec.size(), "Entities and Components containers do not have the same preallocated size");
		//	});

		return rg::count_if(ids, [] (const auto& e) { return e.has_value(); });
	}

	// Be sure to note comment of size()
	auto is_full() /* const */ -> bool {
		const auto sz = size(),
				   ids_size = ids.size();

		SAGE_ASSERT(sz <= ids_size, "size() cannot be > ids.size(), make sure entity creation/deletion is correct");

		return sz == ids_size;
	}

	auto clear() -> void {
		rg::fill(ids, std::nullopt);
	}
};

using ECS = Basic_ECS<_ALL_COMPONENTS>;		// Excessive? maybe, I care? no.

} //sage

template <>
FMT_FORMATTER(sage::entity::ID) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::entity::ID) {
		return fmt::format_to(ctx.out(), "entity::ID: {}", obj);
	}
};

template <>
FMT_FORMATTER(sage::component::Name) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::component::Name) {
		return fmt::format_to(ctx.out(), "{:?}", obj.name);
	}
};
template <>
FMT_FORMATTER(sage::component::Sprite) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::component::Sprite) {
		return fmt::format_to(ctx.out(), "{}", glm::to_string(obj.color));
	}
};
template <>
FMT_FORMATTER(sage::component::Transform) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::component::Transform) {
		return fmt::format_to(ctx.out(), "{}", glm::to_string(obj.trans));
	}
};
template <>
FMT_FORMATTER(sage::component::Camera) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::component::Camera) {
		return fmt::format_to(ctx.out(), "TODO");
	}
};
template <>
FMT_FORMATTER(sage::component::Position) {
	FMT_FORMATTER_DEFAULT_PARSE

	FMT_FORMATTER_FORMAT(sage::component::Position) {
		return fmt::format_to(ctx.out(), "{}", glm::to_string(obj.position));
	}
};

// TODO: FMT_FORMATTER for ECS::Entity, dont forget to print the addresses of the ECS to debug their origin

#ifdef SAGE_TEST_ECS
namespace {

using namespace sage;

TEST_CASE ("Must not compile") {
	//const entity::ID ent = 666;

	//const auto func = [] (const size_t e) {
	//	return e + 1;
	//};
	//func(ent);
}

constexpr auto infinity = std::numeric_limits<float>::infinity();

TEST_CASE ("ECS") {
	struct Physics {
		glm::vec2 velocity;

		SAGE_ECS_TYPE_NAME_GETTER(Physics);
	};

	struct Collision {
		glm::vec2 a, b;

		SAGE_ECS_TYPE_NAME_GETTER(Collision);
	};

	constexpr auto max_entities = 100ul;

	using ECS = sage::Basic_ECS<Physics, Collision>;
	auto ecs = ECS{max_entities};

	auto entities = std::vector<ECS::Entity>{};
	entities.reserve(max_entities);

	// Create
	{
		constexpr auto half_max_entities = size_t{max_entities / 2};

		for ([[maybe_unused]] const auto _ : vw::iota(0ul, half_max_entities)) {
			auto entt = ecs.create();
			REQUIRE(entt.has_value());

			// This is an interesting line of cpp.
			// `entt` is auto& so *entt is also auto& so we need to move it otherwise we copy.
			// However the optional is still considered to `has_value()`, its just that the value
			// has been moved from.
			// https://en.cppreference.com/w/cpp/utility/optional/operator*
			// Note the &,&& on the right side of the functions (operator*() & or operator*() const&)
			// https://godbolt.org/z/rYaGhf9nP
			entities.push_back(std::move(*entt));
		}

		// Set test data
		for (const auto& [i, entt] : entities | vw::enumerate) {
			REQUIRE(entt.is_valid());
			auto comps = entt.set(Physics{{i, i}}, Collision{{i, i}, {i, i}});
			CHECK(comps.has_value());
			auto& ph = std::get<std::optional<Physics>&>(*comps);
			REQUIRE(ph.has_value());
			CHECK_EQ(ph->velocity, glm::vec2{i, i});
		}

		CHECK_EQ(ecs.size(), max_entities);
	}

	const auto first_entity = entities.begin();
	const auto first_entity_id = first_entity->id();

	// View
	auto view = ecs.view<Physics, Collision>();
	REQUIRE(not view.empty());
	rg::for_each(view, [&] (const auto& entt_comps) {
			const auto& [_e, ph, col] = entt_comps;
			REQUIRE((_e.has_value() and ph.has_value() and col.has_value()));

			const auto e = _e.raw();
			CHECK_EQ(e, ph->velocity.x);
			CHECK_EQ(e, ph->velocity.y);
			CHECK_EQ(e, col->a.x);
			CHECK_EQ(e, col->a.y);
			CHECK_EQ(e, col->b.x);
			CHECK_EQ(e, col->b.y);

			// Tweak values of one to confirm they are retained
			if (_e == first_entity->id()) {
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
		const auto bools = first_entity->has<Physics>();
		REQUIRE(bools.has_value());
		CHECK_EQ(std::tuple_size_v<std::decay_t<decltype(*bools)>>, 1);
		CHECK(std::get<0>(*bools));
	}

	{
		const auto bools = first_entity->has<Physics, Collision>();
		REQUIRE(bools.has_value());
		CHECK_EQ(std::tuple_size_v<std::decay_t<decltype(*bools)>>, 2);
		std::apply([] (const auto&... b) { (CHECK(b), ...); }, *bools);
	}

	// Check if changes remain
	const auto components = first_entity->components<Physics, Collision>();
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
	CHECK(ecs.destroy(*first_entity));
	CHECK(first_entity->id() != first_entity_id);
	CHECK_EQ(ecs.size(), max_entities - 1);

	// Clear
	ecs.clear();
	CHECK_EQ(ecs.size(), 0);
	CHECK(ecs.view<Physics, Collision>().empty());
}

}
#endif
