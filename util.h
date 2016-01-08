#pragma once

#ifndef _UTIL_H_
#define _UTIL_H_

#include <random>
#include <sstream>

extern std::random_device generator;

inline float rand(float lower, float upper) {
	return std::uniform_real_distribution<float>(lower, upper)(generator);
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

#endif