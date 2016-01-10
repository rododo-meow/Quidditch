#pragma once

#ifndef _UTIL_H_
#define _UTIL_H_

#include <random>
#include <sstream>

extern std::random_device generator;

inline float rand(float lower, float upper) {
	return std::uniform_real_distribution<float>(lower, upper)(generator);
}

inline size_t rand(size_t lower, size_t upper) {
	return std::uniform_int_distribution<size_t>(lower, upper)(generator);
}

template<typename T>
inline T rand() {
	return std::uniform_int_distribution<T>()(generator);
}

inline std::string ftos(float f) {
	std::ostringstream os;
	os << f;
	return os.str();
}

#define SAFE_DELETE(x) if ((x)) { \
	delete (x); \
	(x) = NULL; \
}

#endif