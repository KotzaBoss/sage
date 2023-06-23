#pragma once

#include <functional>

template <typename T>
struct Ref_Wrapper : std::reference_wrapper<T> {
	using std::reference_wrapper<T>::reference_wrapper;

	inline auto operator-> () -> T* {
		return &this->get();
	}

	inline auto operator* () -> T* {
		return &this->get();
	}
};
