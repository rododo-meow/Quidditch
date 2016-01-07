#pragma once

#ifndef _GAME_PARAM_H_
#define _GAME_PARAM_H_

namespace GameParam {
	static const char *BALL_OBJ_FILENAME = "ObjModel/ball.obj";
	static const char *TABLE_OBJ_FILENAME = "ObjModel/table.obj";
	static const char *FACE_TEXTURE_FILENAME = "ObjModel/face.jpg";

	static const bool FULLSCREEN = false;
	static const int DEFAULT_WINDOW_WIDTH = 640;
	static const int DEFAULT_WINDOW_HEIGHT = 480;
	static const int FPS = 60;

	static const float DEFAULT_VIEW_DISTANCE = 2.f;
	static const float MIN_VIEW_DISTANCE = 0.5f;
	static const float MAX_VIEW_DISTANCE = 4.f;

	static const float TABLE_WIDTH = 0.8f;
	static const float TABLE_LENGTH = 1.7f;
	static const float CORNER_RADIUS = 0.05f;
	static const float TABLE_FACE_Y = -0.02137f;

	static const float BALL_RADIUS = 0.025f;
	static const float INIT_VELOCITY = 0.3f;
	static const float RUNNER_INIT_VELOCITY = INIT_VELOCITY / 1.1f;
	static const float ACCELERATION = INIT_VELOCITY * 1.5;
	static const float RUNNER_ACCELERATION = RUNNER_INIT_VELOCITY / 5;
	static const float TURNING_FACTOR = 1.35f;
	static const float RUNNER_TURNING_FACTOR = 0.3f;
	static const float SPEED_UP_VELOCITY = INIT_VELOCITY * 2.5f;
	static const float SPEED_UP_COOLDOWN = 5.f;
	static const float FLYING_RUSH_VELOCITY = INIT_VELOCITY * 5;
	static const float FLYING_FLOATING_VELOCITY = INIT_VELOCITY * 2;
	static const float FLYING_DOWN_VELOCITY = INIT_VELOCITY * 1.2;
	static const float FLYING_RUSH_P = 0.01f;
	static const float FLYING_RELAX_P = 0.004f;
	static const float FLYING_WAKEUP_P = 0.004f;
	static const float FLYING_MAX_FLY_HEIGHT = 0.2f;
	static const float FLYING_MIN_FLY_HEIGHT = BALL_RADIUS * 2;
	static const float FLYING_MIN_X = -TABLE_LENGTH * 1.5f;
	static const float FLYING_MAX_X = TABLE_LENGTH * 1.5f;
	static const float FLYING_MIN_Z = -TABLE_WIDTH * 1.5f;
	static const float FLYING_MAX_Z = TABLE_WIDTH * 1.5f;
	static const float FLAG_POSITION_X = 0.f;
	static const float FLAG_POSITION_Y = 0.0f;
	static const float FLAG_POSITION_Z = -0.4f;
	static const float FLAG_HEIGHT = 0.7f;
	static const float FLAG_LENGTH = 0.5f;
	static const float FLAG_WIDTH = 0.3f;

	static const int N_RUNNERS = 6;
	static const int N_LAZY = 6;

	static const float MOUSE_SENSITIVIE = 0.05f;

	static const float BASE_SCORE = 3;
	static const float SCORE_FACTOR = 90;
	static const int PENALTY_SCORE = 30;
	static const int FLYING_SCORE = 300;

	static const float SCORE_X = -0.8f;
	static const float SCORE_Y = 0.8f;

	static const float FPS_LABEL_X = SCORE_X;
	static const float FPS_LABEL_Y = SCORE_Y - 0.1f;

	static const float COOLDOWN_HEIGHT = 0.03f;
	static const float COOLDOWN_WIDTH = 0.25f;
	static const float COOLDOWN_X = SCORE_X;
	static const float COOLDOWN_Y = FPS_LABEL_Y - COOLDOWN_HEIGHT * 2;
	static const float COOLDOWN_LEFT_COLOR[4] = { 1, 0, 0, 1 };
	static const float COOLDOWN_RIGHT_COLOR[4] = { 0, 1, 0, 1 };

	static const float GAMEOVER_LABEL_X = -0.4f;
	static const float GAMEOVER_LABEL_Y = 0.1f;

	static const float FADE_OUT_DURATION = 5.f;
	static const float FADE_OUT_ALPHA = .6f;

	static const float G = 5.f;
};

#endif