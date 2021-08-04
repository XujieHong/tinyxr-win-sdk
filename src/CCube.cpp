#include "CCube.h"
#include <iomanip>
#define M_PI       3.14159265358979323846

CCube::CCube()
{
}

CCube::~CCube()
{
}


void CCube::init_shader()
{
    // vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader, NULL);
    glCompileShader(vertexShader);
    // check vertex shader compiling status
    GLint compiled;
    GLchar log[256];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        glGetShaderInfoLog(vertexShader, 256, NULL, log);
    }

    // fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader, NULL);
    glCompileShader(fragmentShader);
    // check fragment shader compiling status
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        glGetShaderInfoLog(fragmentShader, 256, NULL, log);
    }

    // link shaders
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    //check link status
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
	{
            glGetProgramInfoLog(program, 256, NULL, log);
    }
    // delete shader
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void CCube::init_data()
{
	glUseProgram(program);
	glGenVertexArrays(1, VAOs);
	glBindVertexArray(VAOs[0]);

    glGenBuffers(NumBuffers, Buffers);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer), color_buffer, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);


	GLuint u_mvp = glGetUniformLocation(program, "u_mvp");
	GLfloat mvp[4][4] = { 0.5, 0, 0, 0,
						 0, 1, 0, 0,
						 0, 0, 1, 0,
						 0, 0, 0, 1 };

	glUniformMatrix4fv(u_mvp, 1, GL_FALSE, &mvp[0][0]);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CCube::OnSensorEvent(unsigned char * buf)
{
	float accSensorData[3];
	float gyroSensorData[3];
	float magSensorData[3];

	accSensorData[0] = *((float*)(buf + 4));
	accSensorData[1] = *((float*)(buf + 8));
	accSensorData[2] = *((float*)(buf + 12));

	gyroSensorData[0] = *((float*)(buf + 16));
	gyroSensorData[1] = *((float*)(buf + 20));
	gyroSensorData[2] = *((float*)(buf + 24));

	magSensorData[0] = *((float*)(buf + 28));
	magSensorData[1] = *((float*)(buf + 32));
	magSensorData[2] = *((float*)(buf + 36));

	mQuaternion[0] = *((float*)(buf + 44));
	mQuaternion[1] = *((float*)(buf + 48));
	mQuaternion[2] = *((float*)(buf + 52));
	mQuaternion[3] = *((float*)(buf + 56));

//	cout << "acc = " << accSensorData[0] << ", " << accSensorData[1] << ", " << accSensorData[2] << endl;
//	cout << "gyro = " << gyroSensorData[0] << ", " << gyroSensorData[1] << ", " << gyroSensorData[2] << endl;
//	cout << "mag = " << magSensorData[0] << ", " << magSensorData[1] << ", " << magSensorData[2] << endl;
//	cout << "quaternion: w = " << mQuaternion[3] << ",     x = " << mQuaternion[0] << ",     y = " << mQuaternion[1] << ",     z = " << mQuaternion[2] << endl;
}

void CCube::OnCommandResp(unsigned char * buf)
{
	cout << "Resp type = " << buf[8] << "\n";
}

Eigen::Matrix3d CCube::euler2RotationMatrix(const double roll, const double pitch, const double yaw)
{
	Eigen::Vector3d ea0(yaw, pitch, roll);
	Eigen::Matrix3d R;

	R = Eigen::AngleAxisd(ea0[0], Eigen::Vector3d::UnitY())
		* Eigen::AngleAxisd(ea0[1], Eigen::Vector3d::UnitX())
		* Eigen::AngleAxisd(ea0[2], Eigen::Vector3d::UnitZ());

	return R;
}

void CCube::display()
{
	glUseProgram(program);

	glBindVertexArray(VAOs[0]);

	GLuint u_mvp = glGetUniformLocation(program, "u_mvp");
	GLfloat mvp[16] = { 1, 0, 0, 0,
						  0, 1, 0, 0,
						  0, 0, 1, 0,
						  0, 0, 0, 1 };

	Eigen::Quaterniond quaternion(mQuaternion[3], mQuaternion[0], mQuaternion[1], mQuaternion[2]);

	// Sensor poase has a -90 degrees rotation about x - axis.
	Eigen::Matrix3d rotation = quaternion.matrix() * Eigen::AngleAxisd(M_PI / 2, Eigen::Vector3d::UnitX());

	mvp[0] = (GLfloat)rotation(0, 0);
	mvp[1] = (GLfloat)rotation(0, 1);
	mvp[2] = (GLfloat)rotation(0, 2);

	mvp[4] = (GLfloat)rotation(1, 0);
	mvp[5] = (GLfloat)rotation(1, 1);
	mvp[6] = (GLfloat)rotation(1, 2);

	mvp[8] = (GLfloat)rotation(2, 0);
	mvp[9] = (GLfloat)rotation(2, 1);
	mvp[10] = (GLfloat)rotation(2, 2);

	glUniformMatrix4fv(u_mvp, 1, GL_FALSE, &mvp[0]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);

	glutSwapBuffers();
}

void CCube::processNormalKeys(unsigned char key, int x, int y)
{
	CUsbHidDevice *pDevice = CUsbHidDevice::getInstance();

	switch (key)
	{
	case 27:
		exit(0);
		break;

	case 49:
		pDevice->StartSensor();
		break;

	case 50:
		pDevice->StopSensor();
		break;

	default:

		break;
	}
}

void CCube::main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("A rotating cube");

    GLint sample;
    glGetIntegerv(GL_SAMPLE_BUFFERS, &sample);

    glewExperimental = GL_TRUE;
    if (glewInit())
    {
         exit(1);
    }

    init_shader();
    init_data();
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutKeyboardFunc(processNormalKeys);
	glEnable(GL_DEPTH_TEST);
    glutMainLoop();
}

GLuint CCube::program;
float CCube::mQuaternion[4];
GLuint CCube::VAOs[1];