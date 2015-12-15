#include "Phys.h"
#include "Ball.h"
#include "GameParam.h"

using namespace GameParam;

void Phys::update(float deltaTime) {
	std::map<Ball*, bool> collided;

	for (std::map<Ball*, Data*>::iterator it = balls.begin(); it != balls.end(); it++) {
		// update velocity
		it->second->velocity += it->second->force * (float) deltaTime;

		// update position
		it->first->setPosition(it->first->getPosition() + it->second->velocity * (float) deltaTime);

		// set collided to false
		collided[it->first] = false;
	}

	// calculate collision
	for (std::map<Ball*, Data*>::iterator it = balls.begin(); it != balls.end(); it++) {
		if (collided.at(it->first))
			continue;
		Eigen::Vector3f position = it->first->getPosition();

		// with table
		if (it->first->collideWithTable) {
			if ((position.x() - it->first->radius) < -TABLE_LENGTH / 2 || (position.x() + it->first->radius) > TABLE_LENGTH / 2) {
				it->second->velocity.x() = -it->second->velocity.x();
				if ((position.x() - it->first->radius) < -TABLE_LENGTH / 2)
					position.x() = it->first->radius - TABLE_LENGTH / 2;
				if ((position.x() + it->first->radius) > TABLE_LENGTH / 2)
					position.x() = TABLE_LENGTH / 2 - it->first->radius;
				it->first->setPosition(position);
			}
			if ((position.z() - it->first->radius) < -TABLE_WIDTH / 2 || (position.z() + it->first->radius) > TABLE_WIDTH / 2) {
				it->second->velocity.z() = -it->second->velocity.z();
				if ((position.z() - it->first->radius) < -TABLE_WIDTH / 2)
					position.z() = it->first->radius - TABLE_WIDTH / 2;
				if ((position.z() + it->first->radius) > TABLE_WIDTH / 2)
					position.z() = TABLE_WIDTH / 2 - it->first->radius;
				it->first->setPosition(position);
			}
			if (position.y() < TABLE_FACE_Y + it->first->radius) {
				it->second->velocity.y() = 0;
				position.y() = TABLE_FACE_Y + it->first->radius;
				it->first->setPosition(position);
			}
		}

		// with other ball
		for (std::map<Ball*, Data*>::iterator it2 = it; ++it2 != balls.end(); ) {
			if (collided.at(it2->first))
				continue;
			if ((position - it2->first->getPosition()).norm() < it->first->radius + it2->first->radius) {
				Eigen::Vector3f normal = (position - it2->first->getPosition()).normalized();
				Eigen::Vector3f v1 = it->second->velocity, v2 = it2->second->velocity;
				if ((v2 - v1).dot(normal) < 0)
					continue;
				Eigen::Vector3f v11 = normal * v1.dot(normal), v21 = normal * v2.dot(normal);
				Eigen::Vector3f v12 = v1 - v11, v22 = v2 - v21;
				float m1 = it->first->m, m2 = it2->first->m;
				it->second->velocity = ((m1 - m2) * v11 + 2 * m2 * v21) / (m1 + m2) + v12;
				it2->second->velocity = ((m2 - m1) * v21 + 2 * m1 * v11) / (m1 + m2) + v22;
				for (std::list<std::pair<AfterCollision, void *> >::iterator callback_it = afterCollisionCallbacks.begin(); callback_it != afterCollisionCallbacks.end(); callback_it++)
					callback_it->first(callback_it->second, it->first, it2->first);
			}
		}
	}
}