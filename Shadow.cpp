#include "gl.h"
#include <cstdio>
#include "Shadow.h"
#include "Shader.h"

ShadowRenderer::ShadowRenderer(int WINDOW_WIDTH, int WINDOW_HEIGHT) : WINDOW_WIDTH(WINDOW_WIDTH), WINDOW_HEIGHT(WINDOW_HEIGHT) {
	sv_vShader = Shader::loadFromFile(GL_VERTEX_SHADER, "shader/shadow.vert");
	sv_gShader = Shader::loadFromFile(GL_GEOMETRY_SHADER, "shader/shadow.geom");
	sv_fShader = Shader::loadFromFile(GL_FRAGMENT_SHADER, "shader/shadow.frag");
	programSV = new ShaderProgram(3, sv_vShader, sv_gShader, sv_fShader);
	loc_sv_isflag = glGetUniformLocation(programSV->getID(), "isFlag");
	checkGLError(__func__, __LINE__);

	mrt_vShader = Shader::loadFromFile(GL_VERTEX_SHADER, "shader/mrt.vert");
	mrt_fShader = Shader::loadFromFile(GL_FRAGMENT_SHADER, "shader/mrt.frag");
	programMRT = new ShaderProgram(2, mrt_vShader, mrt_fShader);
	loc_texID = glGetUniformLocation(programMRT->getID(), "texID");
	checkGLError(__func__, __LINE__);
	loc_mrt_isflag = glGetUniformLocation(programMRT->getID(), "isFlag");
	checkGLError(__func__, __LINE__);

	rect_vShader = Shader::loadFromFile(GL_VERTEX_SHADER, "shader/rect.vert");
	rect_fShader = Shader::loadFromFile(GL_FRAGMENT_SHADER, "shader/rect.frag");
	programRect = new ShaderProgram(2, rect_vShader, rect_fShader);

	glEnable(GL_LIGHT0);
	CHECK_GL;

	glGenFramebuffers(1, &fbo);
	CHECK_GL;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	CHECK_GL;

	glGenTextures(1, &ambientBuffer);
	CHECK_GL;
	glBindTexture(GL_TEXTURE_2D, ambientBuffer);
	CHECK_GL;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	CHECK_GL;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	CHECK_GL;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	CHECK_GL;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	CHECK_GL;
	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ambientBuffer, 0);
	CHECK_GL;

	glGenTextures(1, &fullLightBuffer);
	CHECK_GL;
	glBindTexture(GL_TEXTURE_2D, fullLightBuffer);
	CHECK_GL;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	CHECK_GL;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	CHECK_GL;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	CHECK_GL;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	CHECK_GL;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, fullLightBuffer, 0);
	CHECK_GL;

	glGenRenderbuffers(1, &depthBuffer);
	CHECK_GL;
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	CHECK_GL;
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT);
	CHECK_GL;
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	CHECK_GL;

	glGenRenderbuffers(1, &stencilBuffer);
	CHECK_GL;
	glBindRenderbuffer(GL_RENDERBUFFER, stencilBuffer);
	CHECK_GL;
	glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX, WINDOW_WIDTH, WINDOW_HEIGHT);
	CHECK_GL;
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilBuffer);
	CHECK_GL;
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	CHECK_GL;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CHECK_GL;

	glGenVertexArrays(1, &vaoRect);
	CHECK_GL;
	glBindVertexArray(vaoRect);
	CHECK_GL;
	glGenBuffers(1, &vboRect);
	CHECK_GL;
	glBindBuffer(GL_ARRAY_BUFFER, vboRect);
	CHECK_GL;
	{
		float data[] = {
			1, -1, 1, 0,
			1, 1, 1, 1,
			-1, -1, 0, 0,
			-1, 1, 0, 1
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	}
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

ShadowRenderer::~ShadowRenderer() {
	glDeleteBuffers(1, &vboRect);
	glDeleteVertexArrays(1, &vaoRect);
	glDeleteRenderbuffers(1, &depthBuffer);
	glDeleteRenderbuffers(1, &stencilBuffer);
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &ambientBuffer);
	glDeleteTextures(1, &fullLightBuffer);
	delete programMRT;
	delete programSV;
	delete programRect;
	delete sv_fShader;
	delete sv_gShader;
	delete sv_vShader;
	delete mrt_vShader;
	delete mrt_fShader;
	delete rect_vShader;
	delete rect_fShader;
}

void ShadowRenderer::stage1() {
	state = STAGE1;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buffers);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	programMRT->enable();
	glUniform1i(loc_texID, 0);
	CHECK_GL;
	glUniform1i(loc_mrt_isflag, GL_FALSE);
	CHECK_GL;
}

void ShadowRenderer::stage2() {
	state = STAGE2;
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LESS);
	glFrontFace(GL_CW);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, 0);
	glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
	glClear(GL_STENCIL_BUFFER_BIT);
	programSV->enable();
	glUniform1i(loc_sv_isflag, GL_FALSE);
}

void ShadowRenderer::stage3() {
	state = STAGE3;
	glFrontFace(GL_CCW);
	glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
}

void ShadowRenderer::end() {
	state = NONE;
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	programSV->disable();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	programRect->enable();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, fullLightBuffer);
	glStencilFunc(GL_EQUAL, 0, 0xFFFFFFFF);

	glBindVertexArray(vaoRect);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	programRect->disable();

	glDisable(GL_STENCIL_TEST);
	glDisable(GL_TEXTURE_2D);
}

void ShadowRenderer::setLightPosition(float x, float y, float z, float w) {
	lightpos[0] = x;
	lightpos[1] = y;
	lightpos[2] = z;
	lightpos[3] = w;
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
}

void ShadowRenderer::setLightAmbient(float r, float g, float b, float a) {
	ambient[0] = r;
	ambient[1] = g;
	ambient[2] = b;
	ambient[3] = a;
}

void ShadowRenderer::setLightDiffuse(float r, float g, float b, float a) {
	diffuse[0] = r;
	diffuse[1] = g;
	diffuse[2] = b;
	diffuse[3] = a;
}

void ShadowRenderer::setLightSpecular(float r, float g, float b, float a) {
	specular[0] = r;
	specular[1] = g;
	specular[2] = b;
	specular[3] = a;
}

void ShadowRenderer::setFlag(bool isFlag) {
	switch (state) {
	case NONE:
		throw std::exception("Can't set flag outside renderer");
	case STAGE1:
		glUniform1i(loc_mrt_isflag, isFlag ? GL_TRUE : GL_FALSE);
		break;
	case STAGE2:
	case STAGE3:
		glUniform1i(loc_sv_isflag, isFlag ? GL_TRUE : GL_FALSE);
		break;
	}
}