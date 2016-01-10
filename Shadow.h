#pragma once

#ifndef _SHADOW_H_
#define _SHADOW_H_

#include "Eigen/Eigen"

class Shader;
class ShaderProgram;
class ShadowRenderer {
	Shader *sv_vShader, *sv_fShader, *sv_gShader, *mrt_vShader, *mrt_fShader, *rect_vShader, *rect_fShader;
	ShaderProgram *programMRT, *programSV, *programRect;
	int loc_texID, loc_mrt_isflag, loc_sv_isflag;
	unsigned int ambientBuffer, fullLightBuffer, spotBuffer, depthBuffer, stencilBuffer, fboMRT, stencilBuffer2;
	const int WINDOW_WIDTH, WINDOW_HEIGHT;
	enum {
		NONE,
		STAGE1,
		STAGE2,
		STAGE3,
		STAGE4
	} state;
	unsigned int vboRect, vaoRect, vboSpotCone, vaoSpotCone;
	bool enableSpot = true;
	Eigen::Vector3f spotPos, spotDirection;
	float spotCutoff;
	void(*renderFunc)(void*);
	void *renderArg;
	void updateSpotCone();
public:
	static const size_t SPOT_CONE_DIVISION = 30;
	ShadowRenderer(int WINDOW_WIDTH, int WINDOW_HEIGHT);
	~ShadowRenderer();
	void stage1();
	void stage2();
	void stage3();
	void stage4();
	void setFlag(bool isFlag);
	inline void setRenderFunc(void(*renderFunc)(void*), void *renderArg) { this->renderFunc = renderFunc; this->renderArg = renderArg; }
	inline void setSpotPos(const Eigen::Vector3f &pos) { spotPos = pos; }
	inline void setSpotDirection(const Eigen::Vector3f &direction) { spotDirection = direction; }
	inline void setSpotCutoff(float angle) { if (angle <= 0 || angle >= 90) throw "Are you kidding?"; spotCutoff = angle; updateSpotCone(); }
};

#endif