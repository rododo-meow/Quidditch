#include "Phys.h"
#include "Ball.h"
#include "GameParam.h"
#include "Table.h"
#include "Game.h"

using namespace GameParam;

#define INF 999.f

Phys::~Phys() {
	for (std::map<Ball*, Data*>::iterator it = balls.begin(); it != balls.end(); it++)
		delete it->second;
}

float triangleDistance(const Eigen::Vector3f P, const Eigen::Vector3f &A, const Eigen::Vector3f &B, const Eigen::Vector3f &C, const Eigen::Vector3f &normal) {
	Eigen::Vector3f v0 = C - A;
	Eigen::Vector3f v1 = B - A;
	Eigen::Vector3f v2 = P - A;

	float dot00 = v0.dot(v0);
	float dot01 = v0.dot(v1);
	float dot02 = v0.dot(v2);
	float dot11 = v1.dot(v1);
	float dot12 = v1.dot(v2);

	float inverDeno = 1 / (dot00 * dot11 - dot01 * dot01);

	float u = (dot11 * dot02 - dot01 * dot12) * inverDeno;
	if (u < 0 || u > 1) // if u out of range, return directly
		return INF;

	float v = (dot00 * dot12 - dot01 * dot02) * inverDeno;
	if (v < 0 || v > 1) // if v out of range, return directly
		return INF;

	if (u + v <= 1)
		return v2.dot(normal);
	else
		return INF;
}

void Phys::update(float deltaTime) {
	for (std::map<Ball*, Data*>::iterator it = balls.begin(); it != balls.end(); it++) {
		// apply gravity
		if (it->first->collideWithTable)
			it->second->velocity += Eigen::Vector3f(0, -1, 0) * G * deltaTime;

		// update velocity
		it->second->velocity += it->second->force * deltaTime;

		// update position
		it->first->setPosition(it->first->getPosition() + it->second->velocity * (float) deltaTime);
	}

	// calculate collision
	for (std::map<Ball*, Data*>::iterator it = balls.begin(); it != balls.end(); it++) {
		Eigen::Vector3f position = it->first->getPosition();

		// with table
		if (it->first->collideWithTable) {
			for (std::list<Table*>::const_iterator it2 = tables.begin(); it2 != tables.end(); it2++) {
				if ((position.x() - it->first->radius) < (*it2)->getPosition().x() - (*it2)->LENGTH / 2) {
					it->second->velocity.x() = -it->second->velocity.x();
					position.x() = it->first->radius - TABLE_LENGTH / 2;
				}
				if ((position.x() + it->first->radius) > (*it2)->getPosition().x() + (*it2)->LENGTH / 2) {
					it->second->velocity.x() = -it->second->velocity.x();
					position.x() = TABLE_LENGTH / 2 - it->first->radius;
				}
				if ((position.z() - it->first->radius) < (*it2)->getPosition().z() - (*it2)->WIDTH / 2) {
					it->second->velocity.z() = -it->second->velocity.z();
					position.z() = it->first->radius - TABLE_WIDTH / 2;
				}
				if ((position.z() + it->first->radius) > (*it2)->getPosition().z() + (*it2)->WIDTH / 2) {
					it->second->velocity.z() = -it->second->velocity.z();
					position.z() = TABLE_WIDTH / 2 - it->first->radius;
				}
				if ((position.y() - it->first->radius - (*it2)->getPosition().y() - TABLE_FACE_Y) < 0)
					position.y() = (*it2)->getPosition().y() + TABLE_FACE_Y + it->first->radius;
				float dis;
#define POS \
	Eigen::Vector3f(position.x() - (*it2)->getPosition().x() + (*it2)->LENGTH / 2, \
		position.y() - (*it2)->getPosition().y() - TABLE_FACE_Y, \
		position.z() - (*it2)->getPosition().z() + (*it2)->WIDTH / 2)
				for (size_t i = floor((POS.z() - it->first->radius) * (*it2)->N_WIDTH / (*it2)->WIDTH),
					leni = ceil((POS.z() + it->first->radius) * (*it2)->N_WIDTH / (*it2)->WIDTH);
					i < leni;
					i++) {
					for (size_t j = floor((POS.x() - it->first->radius) * (*it2)->N_LENGTH / (*it2)->LENGTH),
							lenj = ceil((POS.x() + it->first->radius) * (*it2)->N_LENGTH / (*it2)->LENGTH);
							j < lenj;
						j++) {
#define VERTEX(i, j) \
	((*it2)->vertex[(i) * ((*it2)->N_LENGTH + 1) + (j)])
#define NORMAL(i, j, k) \
	((*it2)->normals[((i) * (*it2)->N_LENGTH + (j)) * 2 + (k)])
						switch ((*it2)->directions[i * (*it2)->N_LENGTH + j]) {
						case Table::ZS_YX:
							if ((dis = triangleDistance(POS, VERTEX(i, j + 1), VERTEX(i, j), VERTEX(i + 1, j), NORMAL(i, j, 0))) < it->first->radius) {
								position += NORMAL(i, j, 0) * (it->first->radius - dis);
								if (it->second->velocity.dot(NORMAL(i, j, 0)) < 0)
									it->second->velocity -= NORMAL(i, j, 0) * NORMAL(i, j, 0).dot(it->second->velocity);
							}
							if ((dis = triangleDistance(POS, VERTEX(i + 1, j), VERTEX(i + 1, j + 1), VERTEX(i, j + 1), NORMAL(i, j, 1))) < it->first->radius) {
								position += NORMAL(i, j, 1) * (it->first->radius - dis);
								if (it->second->velocity.dot(NORMAL(i, j, 1)) < 0)
									it->second->velocity -= NORMAL(i, j, 1) * NORMAL(i, j, 1).dot(it->second->velocity);
							}
							break;
						case Table::YS_ZX:
							if ((dis = triangleDistance(POS, VERTEX(i, j), VERTEX(i + 1, j), VERTEX(i + 1, j + 1), NORMAL(i, j, 0))) < it->first->radius) {
								position += NORMAL(i, j, 0) * (it->first->radius - dis);
								if (it->second->velocity.dot(NORMAL(i, j, 0)) < 0)
									it->second->velocity -= NORMAL(i, j, 0) * NORMAL(i, j, 0).dot(it->second->velocity);
							}
							if ((dis = triangleDistance(POS, VERTEX(i, j), VERTEX(i + 1, j + 1), VERTEX(i, j + 1), NORMAL(i, j, 1))) < it->first->radius) {
								position += NORMAL(i, j, 1) * (it->first->radius - dis);
								if (it->second->velocity.dot(NORMAL(i, j, 1)) < 0)
									it->second->velocity -= NORMAL(i, j, 1) * NORMAL(i, j, 1).dot(it->second->velocity);
							}
							break;
						}
					}
				}
			}
		}

		// with other ball
		for (std::map<Ball*, Data*>::iterator it2 = it; ++it2 != balls.end(); ) {
			if ((position - it2->first->getPosition()).norm() < it->first->radius + it2->first->radius) {
				Eigen::Vector3f normal = (position - it2->first->getPosition()).normalized();
				position += normal * (it->first->radius + it2->first->radius - (position - it2->first->getPosition()).norm());
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

		it->first->setPosition(position);
	}
}