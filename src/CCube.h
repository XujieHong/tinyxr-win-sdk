#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLFW/glfw3.h"
#include "CUsbHidDevice.h"
#include "Eigen/Eigen"
#include "Eigen/Geometry"  
#include "Eigen/Core"

using namespace std;
#define BUFFER_OFFSET(x) ((const void*)(x))



const float vertices[] =
{
		-0.4f, -0.4f, -0.5f,     // front 
		0.4f, -0.4f, -0.5f,
		0.4f,  0.4f, -0.5f,
		0.4f,  0.4f, -0.5f,
		-0.4f,  0.4f, -0.5f,
		-0.4f, -0.4f, -0.5f,

		-0.5f, -0.5f,  0.5f,    // back
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,   // left
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		0.5f,  0.5f,  0.5f,  // right
		0.5f,  0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,  // bottom
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,  // top
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
};

const GLfloat color_buffer[] =
{
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,

	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,

	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,

	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,

	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,

	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
};

class CCube : public CHidRespInterface
{
public:
	CCube();
	virtual ~CCube();

private:
	enum Buffer_IDs { ArrayBuffer, NumBuffers };
	enum Attrib_IDs { vPosition = 0 };

	GLuint Buffers[NumBuffers];

	const GLchar* vertex_shader = "#version 430 core \n"
		                                   "layout(location=0) in vec3 vPosition;\n"
		                                   "layout(location=1) in vec3 color;\n"
		                                   "uniform mat4 u_mvp;\n"
		                                   "out vec3 fragmentC;\n"
		                                   "void main()\n"
		                                   "{\n"
		                                   "gl_Position = u_mvp*vec4(vPosition, 1.0);\n"
		                                   "fragmentC = color;\n"
		                                   "}\n";

	const GLchar* fragment_shader = "#version 430 core \n"
		                                   "in vec3 fragmentC;\n"
		                                   "out vec3 fColor;\n"
		                                   "void main()\n"
		                                   "{\n"
		                                    "fColor = fragmentC;\n"
		                                   "}\n";
	static GLuint program;
	//static double mYaw, mPitch, mRoll;
	static float mQuaternion[4];
	static GLuint VAOs[1];

private:
	void init_shader();
	void init_data();
	void OnSensorEvent(unsigned char *buf);
	void OnCommandResp(unsigned char *buf);

	static void display();
	static void processNormalKeys(unsigned char key, int x, int y);
	static Eigen::Matrix3d euler2RotationMatrix(const double roll, const double pitch, const double yaw);

public:
	void main(int argc, char* argv[]);
};

