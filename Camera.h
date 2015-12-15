#pragma once

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Eigen/Eigen"
#include "Transforms.h"
#include "glMatUtil.h"

class Camera {
	Eigen::Matrix4f &matObservation, &matProjection;
public:
	Camera() : matObservation(*new Eigen::Matrix4f()), matProjection(*new Eigen::Matrix4f()) {
		matObservation.setIdentity();
		matProjection.setIdentity();
	}
	inline void setObservation(const Eigen::Matrix4f &matOb) {
		matObservation = matOb;
	}
	inline void setPerspective(float theta, float aspect, float nearDis, float farDis) {
		matProjection = matPerspective(theta, aspect, nearDis, farDis);
	}
	inline void setUVN(Eigen::Vector3f &N, Eigen::Vector3f &V) {
		Eigen::Vector3f pos = getPosition();
		matObservation = matUVN(N, V);
		setPosition(pos);
	}
	inline void setUVN(Eigen::Vector4f &N, Eigen::Vector4f &V) {
		Eigen::Vector3f N3 = N.topRows(3), V3 = V.topRows(3);
		setUVN(N3, V3);
	}
	inline const Eigen::Vector3f getU() const {
		Eigen::Vector3f ans;
		ans(0, 0) = matObservation(0, 0);
		ans(1, 0) = matObservation(0, 1);
		ans(2, 0) = matObservation(0, 2);
		return ans;
	}
	inline const Eigen::Vector4f getU4() const {
		Eigen::Vector4f ans = matObservation.row(0).transpose();
		ans(3, 0) = 1.f;
		return ans;
	}
	inline const Eigen::Vector3f getV() const {
		Eigen::Vector3f ans;
		ans(0, 0) = matObservation(1, 0);
		ans(1, 0) = matObservation(1, 1);
		ans(2, 0) = matObservation(1, 2);
		return ans;
	}
	inline const Eigen::Vector4f getV4() const {
		Eigen::Vector4f ans = matObservation.row(1).transpose();
		ans(3, 0) = 1.f;
		return ans;
	}
	inline const Eigen::Vector3f getN() const {
		Eigen::Vector3f ans;
		ans(0, 0) = matObservation(2, 0);
		ans(1, 0) = matObservation(2, 1);
		ans(2, 0) = matObservation(2, 2);
		return ans;
	}
	inline const Eigen::Vector4f getN4() const {
		Eigen::Vector4f ans = matObservation.row(2).transpose();
		ans(3, 0) = 1.f;
		return ans;
	}
	inline const Eigen::Matrix4f& getProjectionMat() const { return matProjection; }
	inline const Eigen::Matrix4f& getObservationMat() const { return matObservation; }
	inline const Eigen::Matrix4f getUVN() const {
		Eigen::Matrix4f ans = matObservation;
		ans(0, 3) = ans(1, 3) = ans(2, 3) = ans(3, 0) = ans(3, 1) = ans(3, 2) = 0.f;
		return ans;
	}
	inline void setPosition(const Eigen::Vector3f &pos) {
		matObservation = getUVN() * matTranslate(-pos);
	}
	inline const Eigen::Vector3f getPosition() const {
		Eigen::Matrix4f tmp = matObservation.inverse();
		Eigen::Vector3f ans;
		ans(0, 0) = tmp(0, 3);
		ans(1, 0) = tmp(1, 3);
		ans(2, 0) = tmp(2, 3);
		return ans;
	}
	~Camera() {
		delete &matObservation;
		delete &matProjection;
	}
	inline void forward(float dis) {
		matObservation = matTranslate(0, 0, dis) * matObservation;
	}
	inline void backward(float dis) {
		matObservation = matTranslate(0, 0, -dis) * matObservation;
	}
	inline void left(float dis) {
		matObservation = matTranslate(dis, 0, 0) * matObservation;
	}
	inline void right(float dis) {
		matObservation = matTranslate(-dis, 0, 0) * matObservation;
	}
	inline void up(float dis) {
		matObservation = matTranslate(0, -dis, 0) * matObservation;
	}
	inline void down(float dis) {
		matObservation = matTranslate(0, dis, 0) * matObservation;
	}
	inline void setMatrix() {
		glLoadMatrix(GL_PROJECTION, matProjection * matObservation);
	}
};

#endif