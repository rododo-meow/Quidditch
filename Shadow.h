#pragma once

#ifndef _SHADOW_H_
#define _SHADOW_H_

class Shader;
class ShaderProgram;
class ShadowRenderer {
	Shader *sv_vShader, *sv_fShader, *sv_gShader, *mrt_vShader, *mrt_fShader, *rect_vShader, *rect_fShader;
	ShaderProgram *programMRT, *programSV, *programRect;
	int loc_texID, loc_mrt_isflag, loc_sv_isflag;
	float lightpos[4];
	float ambient[4];
	float diffuse[4];
	float specular[4];
	unsigned int ambientBuffer, fullLightBuffer, depthBuffer, stencilBuffer, fbo;
	const int WINDOW_WIDTH, WINDOW_HEIGHT;
	enum {
		NONE,
		STAGE1,
		STAGE2,
		STAGE3
	} state;
	unsigned int vboRect, vaoRect;
public:
	ShadowRenderer(int WINDOW_WIDTH, int WINDOW_HEIGHT);
	~ShadowRenderer();
	void stage1();
	void stage2();
	void stage3();
	void end();
	void setLightPosition(float x, float y, float z, float w);
	void setLightAmbient(float r, float g, float b, float a);
	void setLightDiffuse(float r, float g, float b, float a);
	void setLightSpecular(float r, float g, float b, float a);
	void setFlag(bool isFlag);
};

#endif