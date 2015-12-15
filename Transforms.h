#pragma once

#ifndef _TRANSFORMS_H_
#define _TRANSFORMS_H_

#include "Eigen/Eigen"

#define F_PI ((float) M_PI)

inline const Eigen::Matrix4f matRotateAroundZ(float angle) {
	Eigen::Matrix4f ans;
	ans.setIdentity();
	float *data = ans.data();
	data[0] = cos(angle * (F_PI / 180.f));
	data[1] = sin(angle * (F_PI / 180.f));
	data[4] = -data[1];
	data[5] = data[0];
	return ans;
}

inline const Eigen::Matrix4f matRotateAroundY(float angle) {
	Eigen::Matrix4f ans;
	ans.setIdentity();
	float *data = ans.data();
	data[0] = cosf(angle * (F_PI / 180.f));
	data[2] = sinf(angle * (F_PI / 180.f));
	data[8] = -data[2];
	data[10] = data[0];
	return ans;
}

inline const Eigen::Matrix4f matRotateAroundX(float angle) {
	Eigen::Matrix4f ans;
	ans.setIdentity();
	float *data = ans.data();
	data[5] = cosf(angle * (F_PI / 180.f));
	data[6] = -sinf(angle * (F_PI / 180.f));
	data[9] = -data[6];
	data[10] = data[5];
	return ans;
}

inline const Eigen::Matrix4f matRotate(const Eigen::Vector3f &axis, float angle) {
	Eigen::Vector3f u = axis.normalized();
	float data[16];
	float cosA = cosf(angle * (F_PI / 180.f));
	float sinA = sinf(angle * (F_PI / 180.f));
	float oneC = 1 - cosA;
	float t = u.x() * oneC;
	data[0] = t * u.x() + cosA;
	data[1] = t * u.y() - u.z() * sinA;
	data[2] = t * u.z() + u.y() * sinA;
	t = u.y() * oneC;
	data[4] = t * u.x() + u.z() * sinA;
	data[5] = t * u.y() + cosA;
	data[6] = t * u.z() - u.x() * sinA;
	t = u.z() * oneC;
	data[8] = t * u.x() - u.y() * sinA;
	data[9] = t * u.y() + u.x() * sinA;
	data[10] = t * u.z() + cosA;
	data[3] = data[7] = data[11] = data[12] = data[13] = data[14] = 0.0f;
	data[15] = 1.0f;
	return Eigen::Matrix4f(data);
}

inline const Eigen::Matrix4f matScale(float x, float y, float z) {
	Eigen::Matrix4f ans;
	ans.setIdentity();
	ans(0, 0) = x;
	ans(1, 1) = y;
	ans(2, 2) = z;
	return ans;
}

inline const Eigen::Matrix4f matScale(const Eigen::Vector3f &scale) {
	Eigen::Matrix4f ans;
	ans.setIdentity();
	ans(0, 0) = scale.x();
	ans(1, 1) = scale.y();
	ans(2, 2) = scale.z();
	return ans;
}

inline const Eigen::Matrix4f matScale(float cx, float cy, float cz, float sx, float sy, float sz) {
	Eigen::Matrix4f ans;
	ans.setIdentity();
	ans(0, 0) = sx;
	ans(1, 1) = sy;
	ans(2, 2) = sz;
	ans(0, 3) = (1 - sx) * cx;
	ans(1, 3) = (1 - sy) * cy;
	ans(2, 3) = (1 - sz) * cz;
	return ans;
}

inline const Eigen::Matrix4f matScale(const Eigen::Vector3f &center, const Eigen::Vector3f &scale) {
	Eigen::Matrix4f ans;
	ans.setIdentity();
	ans(0, 0) = scale.x();
	ans(1, 1) = scale.y();
	ans(2, 2) = scale.z();
	ans(0, 3) = (1 - scale.x()) * center.x();
	ans(1, 3) = (1 - scale.y()) * center.y();
	ans(2, 3) = (1 - scale.z()) * center.z();
	return ans;
}

inline const Eigen::Matrix4f matTranslate(const Eigen::Vector3f &offset) {
	Eigen::Matrix4f ans;
	ans.setIdentity();
	ans(0, 3) = offset.x();
	ans(1, 3) = offset.y();
	ans(2, 3) = offset.z();
	return ans;
}

inline const Eigen::Matrix4f matTranslate(float x, float y, float z) {
	Eigen::Matrix4f ans;
	ans.setIdentity();
	ans(0, 3) = x;
	ans(1, 3) = y;
	ans(2, 3) = z;
	return ans;
}

inline const Eigen::Matrix4f matUVN(const Eigen::Vector3f &N, const Eigen::Vector3f &V) {
	Eigen::Matrix4f ans;
	ans.setIdentity();
	Eigen::Vector3f n = (-N).normalized(), u = V.cross(n).normalized(), v = n.cross(u);
	for (int i = 0; i < 3; i++)
		ans(0, i) = u(i, 0);
	for (int i = 0; i < 3; i++)
		ans(1, i) = v(i, 0);
	for (int i = 0; i < 3; i++)
		ans(2, i) = n(i, 0);
	return ans;
}

inline const Eigen::Matrix4f matLookAt(const Eigen::Vector3f &from, const Eigen::Vector3f &to, const Eigen::Vector3f &up) {
	return matUVN(to - from, up) * matTranslate(-from);
}

inline const Eigen::Matrix4f matPerspective(float theta, float aspect, float nearDis, float farDis) {
	Eigen::Matrix4f ans;
	ans.setZero();
	ans(1, 1) = 1 / tanf(theta * (F_PI / 360));
	ans(0, 0) = ans(1, 1) / aspect;
	ans(2, 2) = (-nearDis - farDis) / (-nearDis + farDis);
	ans(2, 3) = (-2 * nearDis * farDis) / (-nearDis + farDis);
	ans(3, 2) = -1.f;
	return ans;
}

inline const Eigen::Vector4f vec3To4(const Eigen::Vector3f &vec) {
	Eigen::Vector4f ans;
	ans(0, 0) = vec(0, 0);
	ans(1, 0) = vec(1, 0);
	ans(2, 0) = vec(2, 0);
	ans(3, 0) = 1.f;
	return ans;
}

inline const Eigen::Vector3f vec4To3(const Eigen::Vector4f &vec) {
	return vec.topRows(3);
}

#endif
