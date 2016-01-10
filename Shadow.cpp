#include "gl.h"
#include "Shadow.h"
#include "Shader.h"
#include <cmath>
#include "glMatUtil.h"
#include "Transforms.h"

ShadowRenderer::ShadowRenderer(int WINDOW_WIDTH, int WINDOW_HEIGHT) : WINDOW_WIDTH(WINDOW_WIDTH), WINDOW_HEIGHT(WINDOW_HEIGHT) {
	sv_vShader = Shader::loadFromFile(GL_VERTEX_SHADER, "shader/shadow.vert");
	sv_gShader = Shader::loadFromFile(GL_GEOMETRY_SHADER, "shader/shadow.geom");
	sv_fShader = Shader::loadFromFile(GL_FRAGMENT_SHADER, "shader/shadow.frag");
	programSV = new ShaderProgram(3, sv_vShader, sv_gShader, sv_fShader);
	loc_sv_isflag = glGetUniformLocation(programSV->getID(), "isFlag");
	CHECK_GL;

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

	glGenFramebuffers(1, &fboMRT);
	CHECK_GL;
	glBindFramebuffer(GL_FRAMEBUFFER, fboMRT);
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
	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fullLightBuffer, 0);
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
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, ambientBuffer, 0);
	CHECK_GL;

	glGenTextures(1, &spotBuffer);
	CHECK_GL;
	glBindTexture(GL_TEXTURE_2D, spotBuffer);
	CHECK_GL;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	CHECK_GL;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	CHECK_GL;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	CHECK_GL;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	CHECK_GL;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, spotBuffer, 0);
	CHECK_GL;

	glBindTexture(GL_TEXTURE_2D, 0);
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

	glGenRenderbuffers(1, &stencilBuffer2);
	CHECK_GL;
	glBindRenderbuffer(GL_RENDERBUFFER, stencilBuffer2);
	CHECK_GL;
	glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX, WINDOW_WIDTH, WINDOW_HEIGHT);
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

	glGenVertexArrays(1, &vaoSpotCone);
	CHECK_GL;
	glBindVertexArray(vaoSpotCone);
	CHECK_GL;
	glGenBuffers(1, &vboSpotCone);
	CHECK_GL;
	glBindBuffer(GL_ARRAY_BUFFER, vboSpotCone);
	CHECK_GL;
	glBufferData(GL_ARRAY_BUFFER, (4 + SPOT_CONE_DIVISION * 2) * 3 * sizeof(float), NULL, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	spotPos = Eigen::Vector3f(0, 0.8f, 0);
	spotDirection = Eigen::Vector3f(0, -1, 0);
	spotCutoff = 5.f;
	updateSpotCone();
}

void ShadowRenderer::updateSpotCone() {
	struct PerVertex {
		float x, y, z;
	};
	glBindBuffer(GL_ARRAY_BUFFER, vboSpotCone);
	PerVertex *map = (PerVertex *) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY), *map2 = map + 3 + SPOT_CONE_DIVISION * 2;
	map->x = map->y = map->z = 0;
	map++;
	for (size_t i = 0; i <= SPOT_CONE_DIVISION; i++) {
		map->x = 20.f * tan(spotCutoff) * sin((float) (M_PI * 2) * i / SPOT_CONE_DIVISION);
		map->y = 20.f * tan(spotCutoff) * cos((float) (M_PI * 2) * i / SPOT_CONE_DIVISION);
		map->z = -20.f;
		*map2 = *map;
		map++, map2--;
	}
	map2->x = map2->y = 0;
	map2->z = -20.f;
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

ShadowRenderer::~ShadowRenderer() {
	glDeleteBuffers(1, &vboRect);
	glDeleteVertexArrays(1, &vaoRect);
	glDeleteRenderbuffers(1, &depthBuffer);
	glDeleteRenderbuffers(1, &stencilBuffer);
	glDeleteFramebuffers(1, &fboMRT);
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
	// do MRT render, render ambient only to ambientBuffer, full light to fullLightBuffer, spot light to spotBuffer
	state = STAGE1;
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboMRT);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboMRT);
	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, buffers);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	programMRT->enable();
	glUniform1i(loc_texID, 0);
	CHECK_GL;
	glUniform1i(loc_mrt_isflag, GL_FALSE);
	CHECK_GL;
	renderFunc(renderArg);
}

void ShadowRenderer::stage2() {
	// do shadow volume generation and z-fail algorithm for main light, wrtie to stencilBuffer, stencil!=0 means it is in shadow
	state = STAGE2;
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilBuffer);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LESS);
	
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, 0);
	glClear(GL_STENCIL_BUFFER_BIT);
	programSV->enable();
	glUniform1i(loc_sv_isflag, GL_FALSE);
	
	glFrontFace(GL_CW);
	glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
	renderFunc(renderArg);

	glFrontFace(GL_CCW);
	glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
	renderFunc(renderArg);

	programSV->disable();
}

void ShadowRenderer::stage3() {
	// do z-fail algorithm for spot light cone, write to stencilBuffer3, stencil != 0 means it under the spot light
	state = STAGE3;
	if (!enableSpot)
		return;
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilBuffer2);
	glClear(GL_STENCIL_BUFFER_BIT);
	glBindVertexArray(vaoSpotCone);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (spotDirection.x() == 0)
		glMultiMatrix(matLookAt(spotPos, spotPos + spotDirection, spotDirection + Eigen::Vector3f(1, 0, 0)).inverse());
	else if (spotDirection.y() == 0)
		glMultiMatrix(matLookAt(spotPos, spotPos + spotDirection, spotDirection + Eigen::Vector3f(0, 1, 0)).inverse());
	else
		glMultiMatrix(matLookAt(spotPos, spotPos + spotDirection, spotDirection + Eigen::Vector3f(0, 0, 1)).inverse());
	
	glFrontFace(GL_CW);
	glStencilOp(GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 2 + SPOT_CONE_DIVISION);
	glDrawArrays(GL_TRIANGLE_FAN, 2 + SPOT_CONE_DIVISION, 2 + SPOT_CONE_DIVISION);

	glFrontFace(GL_CCW);
	glStencilOp(GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 2 + SPOT_CONE_DIVISION);
	glDrawArrays(GL_TRIANGLE_FAN, 2 + SPOT_CONE_DIVISION, 2 + SPOT_CONE_DIVISION);

	glBindVertexArray(0);
}

void ShadowRenderer::stage4() {
	// combine all color buffer
	state = STAGE4;
	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboMRT);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	programRect->enable();
	glEnable(GL_TEXTURE_2D);
	glStencilFunc(GL_NOTEQUAL, 0, 0xFFFFFFFF);
	glBindVertexArray(vaoRect);
	
	if (enableSpot) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fboMRT);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilBuffer2);
		glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_STENCIL_BUFFER_BIT, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, spotBuffer);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboMRT);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilBuffer);
	glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_STENCIL_BUFFER_BIT, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, ambientBuffer);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	programRect->disable();
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_STENCIL_TEST);
	glDisable(GL_TEXTURE_2D);

	state = NONE;
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