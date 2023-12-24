#pragma once

#include "src/std.hpp"
#include "src/graphics.hpp"
#include "src/time.hpp"

namespace sage::particle {

struct Properties {
	glm::vec2 position, velocity;
	glm::vec4 color;
	float rotation = 0.f, size;
	std::chrono::milliseconds lifetime = 1000ms;
};

namespace system {

struct Base {
	using Clock = std::chrono::steady_clock;

protected:
	struct Particle {
	public:
		Properties properties;

	private:
		Clock::time_point emission_timestamp;

	public:
		Particle(Properties&& ps)
			: properties{std::move(ps)}
			, emission_timestamp{Clock::now()}
		{}

		auto should_die() const -> bool {
			return Clock::now() - emission_timestamp >= properties.lifetime;
		}
	};

private:
	std::vector<Particle> particles;
	size_t max_particles;

	std::vector<time::Lifetime<std::chrono::milliseconds>> lifetimes;	// FIXME: Why the default template for Lifetime doesnt work?

protected:
	Base(const size_t max_particles = 100)
		: max_particles{max_particles}
	{
		particles.reserve(max_particles);
		lifetimes.reserve(max_particles);
	}

protected:
	template <std::invocable<const std::span<Particle>> Impl>
	auto update(const std::chrono::milliseconds dt, Impl&& impl) -> void {
		SAGE_ASSERT(particles.size() == lifetimes.size());

		rg::for_each(lifetimes, [&] (auto& l) { l.tick(dt); });

		// RND: How would this parallel-array conditional transformation work cpu-cache wise?
		//      Is keeping the data in separate arrays enough to have reasonable performance?

		// Erase particles whose lifetime is up and their lifetimes
		for (auto i = 0ul; i < particles.size(); ) {	// Spent too long looking for some std::whatever and failed so a simple loop will do for now...
			if (lifetimes[i].is_dead()) {
				lifetimes.erase(lifetimes.begin() + i);
				particles.erase(particles.begin() + i);
				// Do not ++i because erasing will move the next object to current index
			}
			else
				++i;
		}

		std::invoke(std::forward<Impl>(impl), particles);
	}

	template <std::invocable<const std::span<const Particle>> Impl>
	auto render(Impl&& impl) -> void {
		std::invoke(std::forward<Impl>(impl), particles);
	}

	auto emit(Properties&& p) -> bool {
		if (particles.size() < max_particles) {
			lifetimes.emplace_back(p.lifetime);
			particles.emplace_back(std::move(p));
			return true;
		}
		else
			return false;
	}
};


}// particle::system

}// particle

