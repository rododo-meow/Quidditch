#pragma once

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Eigen/Eigen"
#include <list>

class Object {
public:
	typedef bool(*BeforeChangePosition)(void *that, Object *obj, const Eigen::Vector3f &oldP, const Eigen::Vector3f &newP);
	typedef bool(*BeforeChangeRotation)(void *that, Object *obj, const Eigen::Matrix4f &oldR, const Eigen::Matrix4f &newR);
	typedef void(*PositionChanged)(void *that, Object *obj, const Eigen::Vector3f &oldP, const Eigen::Vector3f &newP);
	typedef void(*RotationChanged)(void *that, Object *obj, const Eigen::Matrix4f &oldR, const Eigen::Matrix4f &newR);
	void *data;
private:
	typedef std::list<std::pair<BeforeChangePosition, void*> > BeforeChangePositionCallbacks;
	typedef std::list<std::pair<BeforeChangeRotation, void*> > BeforeChangeRotationCallbacks;
	typedef std::list<std::pair<PositionChanged, void*> > PositionChangedCallbacks;
	typedef std::list<std::pair<RotationChanged, void*> > RotationChangedCallbacks;
	BeforeChangePositionCallbacks beforeChangePositionCallback;
	BeforeChangeRotationCallbacks beforeChangeRotationCallback;
	PositionChangedCallbacks positionChangedCallback;
	RotationChangedCallbacks rotationChangedCallback;
protected:
	Eigen::Vector3f &position;
	Eigen::Matrix4f &rotation;
	Eigen::Vector3f &scale;
public:
	Object() : position(*new Eigen::Vector3f()), rotation(*new Eigen::Matrix4f()), scale(*new Eigen::Vector3f()) { position.setZero(); rotation.setIdentity(); scale.setOnes(); }
	virtual ~Object() { delete &position; delete &rotation; delete &scale; }
	inline void setPosition(const Eigen::Vector3f &position) {
		for (BeforeChangePositionCallbacks::iterator it = beforeChangePositionCallback.begin(); it != beforeChangePositionCallback.end(); it++)
			if (!it->first(it->second, this, this->position, position))
				return;
		Eigen::Vector3f oldPosition = this->position;
		this->position = position;
		for (PositionChangedCallbacks::iterator it = positionChangedCallback.begin(); it != positionChangedCallback.end(); it++)
			it->first(it->second, this, oldPosition, this->position);
	}
	inline const Eigen::Vector3f& getPosition() const { return position; }
	void setRotation(const Eigen::Matrix4f &rotation) {
		for (BeforeChangeRotationCallbacks::iterator it = beforeChangeRotationCallback.begin(); it != beforeChangeRotationCallback.end(); it++)
			if (!it->first(it->second, this, this->rotation, rotation))
				return;
		Eigen::Matrix4f oldRotation = this->rotation;
		this->rotation = rotation;
		for (RotationChangedCallbacks::iterator it = rotationChangedCallback.begin(); it != rotationChangedCallback.end(); it++)
			it->first(it->second, this, oldRotation, this->rotation);
	}
	inline const Eigen::Matrix4f& getRotation() const { return rotation; }
	inline void setScale(const Eigen::Vector3f &scale) { this->scale = scale; }
	inline const Eigen::Vector3f& getScale() const { return scale; }
	inline void addBeforeChangePosition(BeforeChangePosition callback, void *that) {
		beforeChangePositionCallback.push_back(std::make_pair(callback, that));
	}
	inline void removeBeforeChangePosition(BeforeChangePosition callback, void *that) {
		beforeChangePositionCallback.remove(std::make_pair(callback, that));
	}
	inline void addBeforeChangeRotation(BeforeChangeRotation callback, void *that) {
		beforeChangeRotationCallback.push_back(std::make_pair(callback, that));
	}
	inline void removeBeforeChangeRotation(BeforeChangeRotation callback, void *that) {
		beforeChangeRotationCallback.remove(std::make_pair(callback, that));
	}
	inline void addPositionChanged(PositionChanged callback, void *that) {
		positionChangedCallback.push_back(std::make_pair(callback, that));
	}
	inline void removePositionChanged(PositionChanged callback, void *that) {
		positionChangedCallback.remove(std::make_pair(callback, that));
	}
	inline void addRotationChanged(RotationChanged callback, void *that) {
		rotationChangedCallback.push_back(std::make_pair(callback, that));
	}
	inline void removeRotationChanged(RotationChanged callback, void *that) {
		rotationChangedCallback.remove(std::make_pair(callback, that));
	}
};

#endif