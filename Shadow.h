#pragma once

#ifndef _SHADOW_H_
#define _SHADOW_H_

class Shader;
class ShaderProgram;
class ShadowRenderer {
	Shader *sv_vShader, *sv_fShader, *sv_gShader, *mtr_vShader, *mtr_fShader;
	ShaderProgram *programMTR, *programSV;
	int loc_texID;
	float lightpos[4];
	float ambient[4];
	float diffuse[4];
	float specular[4];
	unsigned int ambientBuffer, fullLightBuffer, depthBuffer, stencilBuffer, fbo;
	const int WINDOW_WIDTH, WINDOW_HEIGHT;
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
};

#endif