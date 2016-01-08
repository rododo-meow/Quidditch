#pragma once

#ifndef _PERLIN_H_
#define _PERLIN_H_

#include <cmath>
#include <functional>

#define EPSILON 0.0000001f

class Generator2D {
public:
	float operator()(float x, float y, size_t seed = 0) {
		return (float) ((double)std::hash<long long>()((((long long)(*(int*)(&x) ^ seed)) << 32) | (long long)(*(int*)(&y) ^ seed)) / (size_t) (~0ULL));
	}
};

class BoundZeroGenerator2D {
public:
	float operator()(float x, float y, size_t seed = 0) {
		if (abs(x) <= EPSILON)
			return 0;
		if (abs(x - 1) <= EPSILON)
			return 0;
		if (abs(y) <= EPSILON)
			return 0;
		if (abs(y - 1) <= EPSILON)
			return 0;
		return Generator2D()(x, y, seed);
	}
};

template<typename Generator>
class LinearInterpolater2D {
public:
	float operator()(float T, float x, float y, size_t seed = 0) {
		float xb = ((int)(x / T)) * T, yb = ((int)(y / T)) * T;
		float r1 = (x - xb) * (Generator()(xb + T, yb, seed) - Generator()(xb, yb, seed)) / T + Generator()(xb, yb, seed),
			r2 = (x - xb) * (Generator()(xb + T, yb + T, seed) - Generator()(xb, yb + T, seed)) / T + Generator()(xb, yb + T, seed);
		return (y - yb) * (r2 - r1) / T + r1;
	}
};

template<typename Interpolater>
class Perlin2D {
	size_t seed;
public:
	Perlin2D() : seed(0) {}
	Perlin2D(size_t seed) : seed(seed) {}
	float operator()(float baseFreq, size_t freqcnt, float baseAmp, float persistence, float x, float y) {
		Interpolater i;
		float freq = baseFreq, ans = 0, amp = baseAmp, ampsum = 0;
		while (freqcnt--) {
			ans += amp * i(1 / freq, x, y, seed);
			ampsum += amp;
			amp *= persistence;
			freq *= 2;
		}
		return ans / ampsum;
	}
};

typedef Perlin2D<LinearInterpolater2D<BoundZeroGenerator2D> > HeightMapPerlin;

#undef EPSILON

#endif