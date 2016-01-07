#pragma once

#ifndef _PERLIN_H_
#define _PERLIN_H_

#include <cmath>
#include <functional>

#define EPSILON 0.0000001f

class BoundZeroGenerator2D {
public:
	float operator()(float x, float y) {
		if (abs(x) <= EPSILON)
			return 0;
		if (abs(x - 1) <= EPSILON)
			return 0;
		if (abs(y) <= EPSILON)
			return 0;
		if (abs(y - 1) <= EPSILON)
			return 0;
		return std::hash<long long>()((((long long)*(int*)(&x)) << 32) | (long long)*(int*)(&y)) / 2147483647.f;
	}
};

template<typename Generator>
class LinearInterpolater2D {
public:
	float operator()(float freq, float x, float y) {
		float xb = ((int)(x / freq)) * freq, yb = ((int)(y / freq)) * freq;
		float r1 = (x - xb) * (Generator()(xb + freq, yb) - Generator()(xb, yb)) / freq + Generator()(xb, yb),
			r2 = (x - xb) * (Generator()(xb + freq, yb + freq) - Generator()(xb, yb + freq)) / freq + Generator()(xb, yb + freq);
		return (y - yb) * (r2 - r1) / freq + r1;
	}
};

template<typename Interpolater>
class Perlin2D {
public:
	float operator()(size_t freqcnt, float persistence, float x, float y) {
		Interpolater i;
		float freq = 1, ans = 0, amp = 1;
		while (freqcnt--) {
			ans += amp * i(freq, x, y);
			amp *= persistence;
			freq /= 2;
		}
		return ans;
	}
};

typedef Perlin2D<LinearInterpolater2D<BoundZeroGenerator2D> > HeightMapPerlin;

#undef EPSILON

#endif