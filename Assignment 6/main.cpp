#include<iostream>
#include<fstream>
#include<stdexcept>
#include<sstream>
#include<cmath>
#include <vector>
#include <array>

//*** In this part make sure the include directory contain those head files ***//
//#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/glu.h"
#include "AntTweakBar.h"
//********//

#include "vmath.h"

#define PI 3.1415926
#define BUFFER_OFFSET(offset) ((void*)(offset))

typedef enum {
	PTS, WIREFRAME, SOLID
} primitive;
typedef enum {
	FLAT, SMOOTH
} shademode;

class Joint {

};
//**** animation code ****// 
class animation {
public:
	int currenttime_ = 1;
	const int maxtime_ = 1200;
	const float fps = 30.0;
	vmath::vec3 planetrans_ = { 0.0f, -5.0f, 0.0f }; // the center of the green plane
	void init() {
		currenttime_ = 1;
	}

	void reset() {
		init();
	}
	void update() {
	}
};

class color {
public:
	vmath::vec4 ambient;
	vmath::vec4 diffuse;
	vmath::vec4 specular;
	float shine;
	color() {
		ambient = vmath::vec4(0, 0, 0, 1);
		diffuse = vmath::vec4(0, 0.5, 0, 1);
		specular = vmath::vec4(0, 0, 0.5, 1);
		shine = 1.0;
	}
};

class model {
public:
	//the green cube
	std::vector<vmath::vec3> cube_ = { { -0.5f, 0.0f, -0.5f }, { -0.5f, 0.0f,
			0.5f }, { -0.5f, 1.0f, -0.5f }, { -0.5f, 1.0f, 0.5f }, { 0.5f, 0.0f,
			-0.5f }, { 0.5f, 0.0f, 0.5f }, { 0.5f, 1.0f, -0.5f }, { 0.5f, 1.0f,
			0.5f } };
	std::vector<vmath::vec3> arm1_ = { { -0.25f, 0.0f, -0.25f }, { -0.25f, 0.0f,
			0.25f }, { -0.25f, 4.0f, -0.25f }, { -0.25f, 4.0f, 0.25f }, { 0.25f,
			0.0f, -0.25f }, { 0.25f, 0.0f, 0.25f }, { 0.25f, 4.0f, -0.25f }, {
			0.25f, 4.0f, 0.25f } };
	std::vector<vmath::vec3> arm2_ = { { -0.25f, 0.0f, -0.25f }, { -0.25f, 0.0f,
			0.25f }, { -0.25f, 3.0f, -0.25f }, { -0.25f, 3.0f, 0.25f }, { 0.25f,
			0.0f, -0.25f }, { 0.25f, 0.0f, 0.25f }, { 0.25f, 3.0f, -0.25f }, {
			0.25f, 3.0f, 0.25f } };
	std::vector<vmath::vec3> arm3_ = { { -0.25f, 0.0f, -0.25f }, { -0.25f, 0.0f,
			0.25f }, { -0.25f, 2.0f, -0.25f }, { -0.25f, 2.0f, 0.25f }, { 0.25f,
			0.0f, -0.25f }, { 0.25f, 0.0f, 0.25f }, { 0.25f, 2.0f, -0.25f }, {
			0.25f, 2.0f, 0.25f } };
	std::vector<std::array<int, 3>> cubefaces_ = { { 0, 6, 4 }, { 0, 2, 6 }, {
			0, 3, 2 }, { 0, 1, 3 }, { 2, 7, 6 }, { 2, 3, 7 }, { 4, 6, 7 }, { 4,
			7, 5 }, { 0, 4, 5 }, { 0, 5, 1 }, { 1, 5, 7 }, { 1, 7, 3 } };
	std::vector<vmath::vec3> cnormals_; // cube normal

// the green plane
	vmath::vec3 planescale_ = vmath::vec3(6.0f, 6.0f, 6.0f);
	const int planetrinum_ = 2;
	std::vector<vmath::vec3> plane_ = { { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f,
			-1.0f }, { -1.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 1.0f }, { -1.0f,
			0.0f, -1.0f }, { -1.0f, 0.0f, 1.0f } };
	std::vector<GLfloat> pnormals_ = { 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f };
	void init() {
		cnormals_.resize(cubefaces_.size());
		for (unsigned i = 0; i < cubefaces_.size(); ++i) {
			vmath::vec3 ab = cube_[cubefaces_[i][1]] - cube_[cubefaces_[i][0]];
			vmath::vec3 ac = cube_[cubefaces_[i][2]] - cube_[cubefaces_[i][0]];
			cnormals_[i] = normalize(cross(ab, ac));
		}
	}
};

class camera {
public:
	vmath::vec3 rot; // This is rotation for camera
	vmath::vec3 trans; // This is translation for camera
	float near_;
	float far_;
	vmath::vec3 lastrot;
	vmath::vec3 lasttrans;
	// transfromation
	vmath::vec3 eye;
	vmath::vec3 u;
	vmath::vec3 v;
	vmath::vec3 w;
	//vmath::mat4 Mc;
	vmath::mat4 M;
	void init() {
		near_ = std::min(0.1, eye[2] + 0.1);
		far_ = std::max(50.0, eye[2] + 40.0);
		reset();
	}
	void reset() {
		rot = vmath::vec3(0, 0, 0);
		trans = vmath::vec3(0, 0, 0);
		u = vmath::vec3(1, 0, 0);
		v = vmath::vec3(0, 1, 0);
		w = vmath::vec3(0, 0, 1);
		eye = vmath::vec3(0, 0, 12);

		lastrot = vmath::vec3(0.0, 0.0, 0.0);
		lasttrans = vmath::vec3(0.0, 0.0, 0.0);
		M = vmath::mat4::identity();
	}
	void update() {
		if (rot[0] != lastrot[0] || rot[1] != lastrot[1]
				|| rot[2] != lastrot[2]) {
			vmath::mat4 rt = vmath::mat4::identity();
			if (rot[0] != lastrot[0]) {
				rt = vmath::rotate(-rot[0] + lastrot[0], vmath::vec3(1, 0, 0));
			} else if (rot[1] != lastrot[1]) {
				rt = vmath::rotate(-rot[1] + lastrot[1], vmath::vec3(0, 1, 0));
			} else if (rot[2] != lastrot[2]) {
				rt = vmath::rotate(-rot[2] + lastrot[2], vmath::vec3(0, 0, 1));
			}
			M = rt * M;
			lastrot = rot;
		}
		if (trans[0] != lasttrans[0] || trans[1] != lasttrans[1]
				|| trans[2] != lasttrans[2]) {
			vmath::vec3 tt = vmath::vec3(0, 0, 0);
			if (trans[0] != lasttrans[0]) {
				tt = -(trans[0] - lasttrans[0]) * u;
			} else if (trans[1] != lasttrans[1]) {
				tt = -(trans[1] - lasttrans[1]) * v;
			} else if (trans[2] != lasttrans[2]) {
				tt = -(trans[2] - lasttrans[2]) * w;
			}
			vmath::mat4 mt = vmath::translate(tt);
			M = mt * M;
			//eye = vmath::vec3(M[3][0], M[3][1], M[3][1] + 15);
			lasttrans = trans;
		}
	}
};
class rotation {
public:
	vmath::vec3 rot1;
	vmath::vec3 rot2;
	vmath::vec3 rot3;
	vmath::vec3 targetEndEffect;
	bool reset_;
	void init() {
		reset_ = false;
		reset();
	}
	void reset() {
		rot1 = vmath::vec3(0.0, 0.0, 0);
		rot2 = vmath::vec3(0.0, 0.0, 0);
		rot3 = vmath::vec3(0.0, 0.0, 0);
		targetEndEffect = vmath::vec3(1.0, 3.0, 2.0);

	}
};
class scene {
public:
	bool cw;
	primitive prim;
	shademode shading;

	bool reset_;
	camera camera_;
	rotation rotation_;
	animation animate_;

// light para
	bool lightson;
	color light1;
	color light2;

	scene() {
		init();
	}
	void init() {
		cw = false; // using GL_CW
		prim = SOLID;
		shading = FLAT;
		reset_ = false;
		camera_.init();
		rotation_.init();
		animate_.init();
		reset();
	}
	void reset() {
		camera_.reset();
		rotation_.reset();
		animate_.reset();

		lightson = true;
		light1.ambient = vmath::vec4(0.5, 0.5, 0.5, 1.0);
		light1.diffuse = vmath::vec4(0.5, 0.5, 0.5, 1.0);
		light1.specular = vmath::vec4(0.1, 0.1, 0.1, 1.0);
		light2.ambient = vmath::vec4(0.5, 0.5, 0.5, 1.0);
		light2.diffuse = vmath::vec4(0.5, 0.5, 0.5, 1.0);
		light2.specular = vmath::vec4(0.1, 0.1, 0.1, 1.0);
	}
};

model gmodel_;
scene g_scene_;

void TW_CALL CopyStdStringToClient(std::string & dst, const std::string & src) {
	dst = src;
}

void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	TwWindowSize(width, height);
}

void setmaterial(std::string whichone) {
	vmath::vec4 ambient, diffuse, specular;
	GLfloat shine = 0.5;
	if (whichone == "cube") {
		ambient = vmath::vec4(0.0, 0.5, 0.0, 1.0);
		diffuse = vmath::vec4(0.0, 0.5, 0.0, 1.0);
		specular = vmath::vec4(0.0, 0.0, 0.0, 1.0);
	} else if (whichone == "cube gray") {
		ambient = vmath::vec4(0.5, 0.5, 0.5, 1.0);
		diffuse = vmath::vec4(0.5, 0.5, 0.5, 1.0);
		specular = vmath::vec4(0.0, 0.0, 0.0, 1.0);
	} else if (whichone == "joint") {
		ambient = vmath::vec4(0.6, 0.4, 0.0, 1.0);
		diffuse = vmath::vec4(0.4, 0.6, 0.0, 1.0);
		specular = vmath::vec4(0.0, 0.0, 0.0, 1.0);
	} else if (whichone == "arm1") {
		ambient = vmath::vec4(0.3, 0.0, 0.7, 0.0);
		diffuse = vmath::vec4(0.3, 0.0, 0.7, 0.0);
		specular = vmath::vec4(0.0, 0.0, 0.0, 1.0);
	} else if (whichone == "arm2") {
		ambient = vmath::vec4(0.6, 0.0, 0.4, 0.0);
		diffuse = vmath::vec4(0.6, 0.0, 0.4, 0.0);
		specular = vmath::vec4(0.0, 0.0, 0.0, 1.0);
	} else if (whichone == "arm3") {
		ambient = vmath::vec4(0.8, 0.0, 0.2, 0.0);
		diffuse = vmath::vec4(0.8, 0.0, 0.2, 0.0);
		specular = vmath::vec4(0.0, 0.0, 0.0, 1.0);
	} else if (whichone == "plane") {
		ambient = vmath::vec4(0.0, 0.5, 0.0, 1.0);
		diffuse = vmath::vec4(0.0, 0.5, 0.0, 1.0);
		specular = vmath::vec4(0.0, 0.0, 0.0, 1.0);
	} else {
		ambient = vmath::vec4(0.0, 0.5, 0.5, 1.0);
		diffuse = vmath::vec4(0.0, 0.5, 0.5, 1.0);
		specular = vmath::vec4(0.0, 0.0, 0.0, 1.0);
	}
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shine);
}

void lightsInitialize() {
	GLfloat light0_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light0_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light0_specular[] = { 0.1, 0.1, 0.1, 1.0 };

	GLfloat light1_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light1_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light1_specular[] = { 0.1, 0.1, 0.1, 1.0 };

	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
}
vmath::vec4 rotateX(vmath::vec4 vec, float angle) {
	vmath::vec4 col1 = vmath::vec4(1.0, 0, 0, 0);
	vmath::vec4 col2 = vmath::vec4(0, cos(angle), sin(angle), 0);
	vmath::vec4 col3 = vmath::vec4(0, -1 * sin(angle), cos(angle), 0);
	vmath::vec4 col4 = vmath::vec4(0, 0, 0, 1);
	vmath::vec4 result = vmath::vec4(
			(col1[0] * vec[0] + col2[0] * vec[1] + col3[0] * vec[2]
					+ col4[0] * vec[3]),
			(col1[1] * vec[0] + col2[1] * vec[1] + col3[1] * vec[2]
					+ col4[1] * vec[3]),
			(col1[2] * vec[0] + col2[2] * vec[1] + col3[2] * vec[2]
					+ col4[2] * vec[3]),
			(col1[3] * vec[0] + col2[3] * vec[1] + col3[3] * vec[2]
					+ col4[3] * vec[3]));
	return result;
}
vmath::vec4 rotateY(vmath::vec4 vec, float angle) {
	vmath::vec4 col1 = vmath::vec4(cos(angle), 0, -1 * sin(angle), 0);
	vmath::vec4 col2 = vmath::vec4(0, 1, 0, 0);
	vmath::vec4 col3 = vmath::vec4(sin(angle), 0, cos(angle), 0);
	vmath::vec4 col4 = vmath::vec4(0, 0, 0, 1);
	vmath::vec4 result = vmath::vec4(
			(col1[0] * vec[0] + col2[0] * vec[1] + col3[0] * vec[2]
					+ col4[0] * vec[3]),
			(col1[1] * vec[0] + col2[1] * vec[1] + col3[1] * vec[2]
					+ col4[1] * vec[3]),
			(col1[2] * vec[0] + col2[2] * vec[1] + col3[2] * vec[2]
					+ col4[2] * vec[3]),
			(col1[3] * vec[0] + col2[3] * vec[1] + col3[3] * vec[2]
					+ col4[3] * vec[3]));
	return result;
}
vmath::vec4 rotateZ(vmath::vec4 vec, float angle) {
	vmath::vec4 col1 = vmath::vec4(cos(angle), sin(angle), 0, 0);
	vmath::vec4 col2 = vmath::vec4(-1 * sin(angle), cos(angle), 0, 0);
	vmath::vec4 col3 = vmath::vec4(0, 0, 1, 0);
	vmath::vec4 col4 = vmath::vec4(0, 0, 0, 1);
	vmath::vec4 result = vmath::vec4(
			(col1[0] * vec[0] + col2[0] * vec[1] + col3[0] * vec[2]
					+ col4[0] * vec[3]),
			(col1[1] * vec[0] + col2[1] * vec[1] + col3[1] * vec[2]
					+ col4[1] * vec[3]),
			(col1[2] * vec[0] + col2[2] * vec[1] + col3[2] * vec[2]
					+ col4[2] * vec[3]),
			(col1[3] * vec[0] + col2[3] * vec[1] + col3[3] * vec[2]
					+ col4[3] * vec[3]));
	return result;
}
vmath::vec4 translatePosition(vmath::vec4 vec, vmath::vec4 translatePos) {
	vmath::vec4 col1 = vmath::vec4(1, 0, 0, 0);
	vmath::vec4 col2 = vmath::vec4(0, 1, 0, 0);
	vmath::vec4 col3 = vmath::vec4(0, 0, 1, 0);
	vmath::vec4 col4 = vmath::vec4(translatePos[0], translatePos[1],
			translatePos[2], 1);
	vmath::vec4 result = vmath::vec4(
			(col1[0] * vec[0] + col2[0] * vec[1] + col3[0] * vec[2]
					+ col4[0] * vec[3]),
			(col1[1] * vec[0] + col2[1] * vec[1] + col3[1] * vec[2]
					+ col4[1] * vec[3]),
			(col1[2] * vec[0] + col2[2] * vec[1] + col3[2] * vec[2]
					+ col4[2] * vec[3]),
			(col1[3] * vec[0] + col2[3] * vec[1] + col3[3] * vec[2]
					+ col4[3] * vec[3]));
	return result;
}
void lightsPosition() {
	GLfloat light0_position[] = { 0.0, 10.0, 3.0, 1.0 };
	GLfloat light1_position[] = { -5.0, -10.0, 2.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
}

void display() {
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	glDepthFunc(GL_LEQUAL);

	switch (g_scene_.prim) {
	case PTS:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glPointSize(1.0f);
		break;
	case WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(1.0f);
		break;
	case SOLID:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	default:
		std::cout << "error prim type!\n";
	}

	if (g_scene_.cw) {
		glFrontFace(GL_CW);
	} else {
		glFrontFace(GL_CCW);
	}
	if (g_scene_.reset_) {
		g_scene_.reset();
		g_scene_.reset_ = false;
	}
	if (g_scene_.rotation_.reset_) {
		g_scene_.rotation_.reset();
		g_scene_.rotation_.reset_ = false;
	}

	switch (g_scene_.shading) {
	case FLAT:
		glShadeModel(GL_FLAT);
		break;
	case SMOOTH:
		glShadeModel(GL_SMOOTH);
		break;
	default:
		std::cout << "error shading mode\n";
	}
	if (g_scene_.lightson) {
		lightsInitialize();
		lightsPosition();
	} else {
		glDisable(GL_LIGHTING);
		glColor3f(0.4f, 0.0f, 0.6f);
	}
	//projection matrix
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (double) width / (double) height, g_scene_.camera_.near_,
			g_scene_.camera_.far_);
// set modelviewmatrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// set viewing matrix
	g_scene_.camera_.update();
	vmath::mat4 Mlookat = vmath::lookat(g_scene_.camera_.eye,
			g_scene_.camera_.eye - vmath::vec3(0, 0, 1), vmath::vec3(0, 1, 0));
	Mlookat = g_scene_.camera_.M * Mlookat;
	GLfloat m[16];
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			m[4 * i + j] = Mlookat[i][j];
		}
	}
	glMultMatrixf(m);

	//draw the green cube;
	glPushMatrix();
	setmaterial("cube");
	glTranslatef(0.0f, g_scene_.animate_.planetrans_[1], 0.0f);
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	for (unsigned i = 0; i < gmodel_.cubefaces_.size(); ++i) {
		glNormal3f(gmodel_.cnormals_[i][0], gmodel_.cnormals_[i][1],
				gmodel_.cnormals_[i][2]);
		for (int j = 0; j < 3; ++j) {
			glVertex3f(gmodel_.cube_[gmodel_.cubefaces_[i][j]][0],
					gmodel_.cube_[gmodel_.cubefaces_[i][j]][1],
					gmodel_.cube_[gmodel_.cubefaces_[i][j]][2]);
		}
	}
	glEnd();
	glPopMatrix();

	//draw the gray cube;
	glPushMatrix();
	setmaterial("cube gray");
	glTranslatef(g_scene_.rotation_.targetEndEffect[0],
			g_scene_.rotation_.targetEndEffect[1],
			g_scene_.rotation_.targetEndEffect[2]);
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	for (unsigned i = 0; i < gmodel_.cubefaces_.size(); ++i) {
		glNormal3f(gmodel_.cnormals_[i][0], gmodel_.cnormals_[i][1],
				gmodel_.cnormals_[i][2]);
		for (int j = 0; j < 3; ++j) {
			glVertex3f(gmodel_.cube_[gmodel_.cubefaces_[i][j]][0],
					gmodel_.cube_[gmodel_.cubefaces_[i][j]][1],
					gmodel_.cube_[gmodel_.cubefaces_[i][j]][2]);
		}
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	setmaterial("arm1");
	glColor3f(0.5f, 0.5f, 0.5f);

	glTranslatef(0.0f, g_scene_.animate_.planetrans_[1] + 1.0, 0.0f);
	glRotatef(g_scene_.rotation_.rot1[0], 1, 0, 0);

	glRotatef(g_scene_.rotation_.rot1[2], 0, 0, 1);
	glRotatef(g_scene_.rotation_.rot1[1], 0, 1, 0);

	glPushMatrix();
	glBegin(GL_TRIANGLES);
	for (unsigned i = 0; i < gmodel_.cubefaces_.size(); ++i) {
		glNormal3f(gmodel_.cnormals_[i][0], gmodel_.cnormals_[i][1],
				gmodel_.cnormals_[i][2]);
		for (int j = 0; j < 3; ++j) {
			glVertex3f(gmodel_.arm1_[gmodel_.cubefaces_[i][j]][0],
					gmodel_.arm1_[gmodel_.cubefaces_[i][j]][1],
					gmodel_.arm1_[gmodel_.cubefaces_[i][j]][2]);

		}
	}
	glEnd();

	setmaterial("arm2");
	glColor3f(0.5f, 0.5f, 0.5f);

	glTranslatef(0.0f, 4.0, 0.0f);
	glRotatef(g_scene_.rotation_.rot2[0], 1, 0, 0);
	glRotatef(g_scene_.rotation_.rot2[2], 0, 0, 1);
	glRotatef(g_scene_.rotation_.rot2[1], 0, 1, 0);
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	for (unsigned i = 0; i < gmodel_.cubefaces_.size(); ++i) {
		glNormal3f(gmodel_.cnormals_[i][0], gmodel_.cnormals_[i][1],
				gmodel_.cnormals_[i][2]);
		for (int j = 0; j < 3; ++j) {
			glVertex3f(gmodel_.arm2_[gmodel_.cubefaces_[i][j]][0],
					gmodel_.arm2_[gmodel_.cubefaces_[i][j]][1],
					gmodel_.arm2_[gmodel_.cubefaces_[i][j]][2]);

		}
	}
	glEnd();

	setmaterial("arm3");
	glColor3f(0.5f, 0.5f, 0.5f);
	glTranslatef(0.0f, 3.0, 0.0f);
	glRotatef(g_scene_.rotation_.rot3[0], 1, 0, 0);

	glRotatef(g_scene_.rotation_.rot3[2], 0, 0, 1);
	glRotatef(g_scene_.rotation_.rot3[1], 0, 1, 0);
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	for (unsigned i = 0; i < gmodel_.cubefaces_.size(); ++i) {
		glNormal3f(gmodel_.cnormals_[i][0], gmodel_.cnormals_[i][1],
				gmodel_.cnormals_[i][2]);
		for (int j = 0; j < 3; ++j) {
			glVertex3f(gmodel_.arm3_[gmodel_.cubefaces_[i][j]][0],
					gmodel_.arm3_[gmodel_.cubefaces_[i][j]][1],
					gmodel_.arm3_[gmodel_.cubefaces_[i][j]][2]);

		}
	}
	glEnd();

	glPopMatrix();
	//draw the blue cube;

	//glTranslatef(0.0f, g_scene_.animate_.planetrans_[1], 0.0f);
	vmath::vec4 endFactor = vmath::vec4(0.0f, 2.0, 0.0f, 1.0f);
	vmath::vec4 final2 = vmath::vec4(0.0f, 4.0, 0.0f, 1.0f);
	vmath::vec4 final3 = vmath::vec4(0.0f, 3.0, 0.0f, 1.0f);
	vmath::vec4 final4 = vmath::vec4(0.0f, 7.0, 0.0f, 1.0f);
	vmath::vec4 final5 = vmath::vec4(0.0f, 9.0, 0.0f, 1.0f);
	endFactor = rotateY(endFactor, g_scene_.rotation_.rot3[1] * PI / 180);
	endFactor = rotateZ(endFactor, g_scene_.rotation_.rot3[2] * PI / 180);
	endFactor = rotateX(endFactor, g_scene_.rotation_.rot3[0] * PI / 180);

	endFactor = translatePosition(endFactor, vmath::vec4(0, 3.0, 0.0, 1));

	endFactor = rotateY(endFactor, (g_scene_.rotation_.rot2[1]) * PI / 180);
	endFactor = rotateZ(endFactor, (g_scene_.rotation_.rot2[2]) * PI / 180);
	endFactor = rotateX(endFactor, (g_scene_.rotation_.rot2[0]) * PI / 180);
	endFactor = translatePosition(endFactor, vmath::vec4(0, 4, 0, 1));
	endFactor = rotateY(endFactor, (g_scene_.rotation_.rot1[1]) * PI / 180);
	endFactor = rotateZ(endFactor, (g_scene_.rotation_.rot1[2]) * PI / 180);
	endFactor = rotateX(endFactor, (g_scene_.rotation_.rot1[0]) * PI / 180);

	//draw the gray cube;
	glPushMatrix();
	setmaterial("cube gray");
	glTranslatef(g_scene_.rotation_.targetEndEffect[0],
			g_scene_.rotation_.targetEndEffect[1],
			g_scene_.rotation_.targetEndEffect[2]);
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	for (unsigned i = 0; i < gmodel_.cubefaces_.size(); ++i) {
		glNormal3f(gmodel_.cnormals_[i][0], gmodel_.cnormals_[i][1],
				gmodel_.cnormals_[i][2]);
		for (int j = 0; j < 3; ++j) {
			glVertex3f(gmodel_.cube_[gmodel_.cubefaces_[i][j]][0],
					gmodel_.cube_[gmodel_.cubefaces_[i][j]][1],
					gmodel_.cube_[gmodel_.cubefaces_[i][j]][2]);
		}
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();

	glPopMatrix();

	glPushMatrix();

	glPopMatrix();

	//draw the green plane;
	glPushMatrix();
	setmaterial("plane");
	glTranslatef(g_scene_.animate_.planetrans_[0],
			g_scene_.animate_.planetrans_[1], g_scene_.animate_.planetrans_[2]);
	glScalef(gmodel_.planescale_[0], gmodel_.planescale_[1],
			gmodel_.planescale_[2]);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 2; ++i) {
		glNormal3f(gmodel_.pnormals_[3 * i], gmodel_.pnormals_[3 * i + 1],
				gmodel_.pnormals_[3 * i + 2]);
		for (int j = 0; j < 3; ++j) {
			glVertex3f(gmodel_.plane_[3 * i + j][0],
					gmodel_.plane_[3 * i + j][1], gmodel_.plane_[3 * i + j][2]);
		}
	}
	glEnd();
	glPopMatrix();

	//calculateAlpha();




	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	TwDraw();
	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}
void calculateAlpha(){
	for(int i=1;i<=3;i++){
		for(int j=1;j<=3;j++){
			//phi(i,j)[j]*(g_scene_.rotation_.targetEndEffect[j]-endFactor[j]);
		}
	}
}
vmath::vec3 crossproduct(vmath::vec4 a, vmath::vec4 b) {
	vmath::vec3 result = vmath::vec3(a[1] * b[2] - a[2] * b[1],
			a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]);
	return result;
}
vmath::vec3 phi(int p, int q) {
	vmath::vec4 end = vmath::vec4(g_scene_.rotation_.targetEndEffect[0],
			g_scene_.rotation_.targetEndEffect[1],
			g_scene_.rotation_.targetEndEffect[2], 1);

	vmath::vec4 a1 = vmath::vec4(1, 0, 0, 0);
	vmath::vec4 a2 = vmath::vec4(0, 1, 0, 0);
	vmath::vec4 a3 = vmath::vec4(0, 0, 1, 0);
	vmath::vec4 r = vmath::vec4(0, 0, 0, 0);

	if (p == 1) {
		if (q == 2) {
			a2 = rotateX(rotateZ(a2, g_scene_.rotation_.rot1[2]),
					g_scene_.rotation_.rot1[1]);
			return (crossproduct(a2, (end - r)));;
		}
		if (q == 3) {
			a3 = rotateX(a3, g_scene_.rotation_.rot1[1]);
			return (crossproduct(a3, (end - r)));
		}
		if (q == 1) {

			return (crossproduct(a1, (end - r)));
		}
	}
	if (p == 2) {
		if (q == 2) {
			a2 = rotateX(rotateZ(a2, g_scene_.rotation_.rot2[2]),
					g_scene_.rotation_.rot2[1]);
			a2 = rotateY(a2, g_scene_.rotation_.rot1[1]);
			a2 = rotateZ(a2, g_scene_.rotation_.rot1[2]);
			a2 = rotateX(a2, g_scene_.rotation_.rot1[0]);
			r = translatePosition(r, vmath::vec4(0, 3, 0, 1));
			r = rotateY(r, g_scene_.rotation_.rot1[1]);
			r = rotateZ(r, g_scene_.rotation_.rot1[2]);
			r = rotateX(r, g_scene_.rotation_.rot1[0]);

			return (crossproduct(a2, (end - r)));
		}
		if (q == 3) {
			a3 = rotateX(a3, g_scene_.rotation_.rot2[1]);

			a3 = rotateY(a3, g_scene_.rotation_.rot1[1]);
			a3 = rotateZ(a3, g_scene_.rotation_.rot1[2]);
			a3 = rotateX(a3, g_scene_.rotation_.rot1[0]);
			r = translatePosition(r, vmath::vec4(0, 3, 0, 1));
			r = rotateY(r, g_scene_.rotation_.rot1[1]);
			r = rotateZ(r, g_scene_.rotation_.rot1[2]);
			r = rotateX(r, g_scene_.rotation_.rot1[0]);

			return (crossproduct(a3, (end - r)));
		}
		if (q == 1) {

			a1 = rotateY(a1, g_scene_.rotation_.rot1[1]);
			a1 = rotateZ(a1, g_scene_.rotation_.rot1[2]);
			a1 = rotateX(a1, g_scene_.rotation_.rot1[0]);
			r = translatePosition(r, vmath::vec4(0, 3, 0, 1));
			r = rotateY(r, g_scene_.rotation_.rot1[1]);
			r = rotateZ(r, g_scene_.rotation_.rot1[2]);
			r = rotateX(r, g_scene_.rotation_.rot1[0]);

			return (crossproduct(a1, (end - r)));
		}
	}
	if (p == 3) {
		if (q == 2) {
			a2 = rotateX(rotateZ(a2, g_scene_.rotation_.rot3[2]),
					g_scene_.rotation_.rot3[1]);
			a2 = rotateY(a2, g_scene_.rotation_.rot2[1]);
			a2 = rotateZ(a2, g_scene_.rotation_.rot2[2]);
			a2 = rotateX(a2, g_scene_.rotation_.rot2[0]);
			a2 = rotateY(a2, g_scene_.rotation_.rot1[1]);
			a2 = rotateZ(a2, g_scene_.rotation_.rot1[2]);
			a2 = rotateX(a2, g_scene_.rotation_.rot1[0]);
			r = translatePosition(r, vmath::vec4(0, 2, 0, 1));
			r = rotateY(r, g_scene_.rotation_.rot2[1]);
			r = rotateZ(r, g_scene_.rotation_.rot2[2]);
			r = rotateX(r, g_scene_.rotation_.rot2[0]);
			r = translatePosition(r, vmath::vec4(0, 3, 0, 1));
			r = rotateY(r, g_scene_.rotation_.rot1[1]);
			r = rotateZ(r, g_scene_.rotation_.rot1[2]);
			r = rotateX(r, g_scene_.rotation_.rot1[0]);

			return (crossproduct(a2, (end - r)));
		}
		if (q == 3) {
			a3 = rotateX(a3,
										g_scene_.rotation_.rot3[1]);
								a3 = rotateY(a3, g_scene_.rotation_.rot2[1]);
								a3 = rotateZ(a3, g_scene_.rotation_.rot2[2]);
								a3 = rotateX(a3, g_scene_.rotation_.rot2[0]);
								a3 = rotateY(a3, g_scene_.rotation_.rot1[1]);
								a3 = rotateZ(a3, g_scene_.rotation_.rot1[2]);
								a3 = rotateX(a3, g_scene_.rotation_.rot1[0]);
								r = translatePosition(r, vmath::vec4(0, 2, 0, 1));
								r = rotateY(r, g_scene_.rotation_.rot2[1]);
								r = rotateZ(r, g_scene_.rotation_.rot2[2]);
								r = rotateX(r, g_scene_.rotation_.rot2[0]);
								r = translatePosition(r, vmath::vec4(0, 3, 0, 1));
								r = rotateY(r, g_scene_.rotation_.rot1[1]);
								r = rotateZ(r, g_scene_.rotation_.rot1[2]);
								r = rotateX(r, g_scene_.rotation_.rot1[0]);

								return (crossproduct(a3, (end - r)));


		}
		if (q == 1) {


								a1 = rotateY(a1, g_scene_.rotation_.rot2[1]);
								a1 = rotateZ(a1, g_scene_.rotation_.rot2[2]);
								a1 = rotateX(a1, g_scene_.rotation_.rot2[0]);
								a1 = rotateY(a1, g_scene_.rotation_.rot1[1]);
								a1 = rotateZ(a1, g_scene_.rotation_.rot1[2]);
								a1 = rotateX(a1, g_scene_.rotation_.rot1[0]);
								r = translatePosition(r, vmath::vec4(0, 2, 0, 1));
								r = rotateY(r, g_scene_.rotation_.rot2[1]);
								r = rotateZ(r, g_scene_.rotation_.rot2[2]);
								r = rotateX(r, g_scene_.rotation_.rot2[0]);
								r = translatePosition(r, vmath::vec4(0, 3, 0, 1));
								r = rotateY(r, g_scene_.rotation_.rot1[1]);
								r = rotateZ(r, g_scene_.rotation_.rot1[2]);
								r = rotateX(r, g_scene_.rotation_.rot1[0]);

								return (crossproduct(a1, (end - r)));
		}
	}

}
float dotproduct(vmath::vec3 a,vmath::vec3 b){
	return(a[0]*b[0]+a[1]*b[1]+a[2]*b[2]);

}

void init() {
	gmodel_.init();
}

void Terminate(void) {
	TwTerminate();
}
void reset() {
	g_scene_.reset();
}

void init_gui() {
	TwBar *operbar = TwNewBar("ControlBar");
	TwDefine(" GLOBAL help='CubeAnimation' ");
	TwDefine(" ControlBar position='16 16' size='200 300' color='96 216 224' ");
	TwAddVarRW(operbar, "CW/CCW", TW_TYPE_BOOL8, &g_scene_.cw, NULL);
	TwEnumVal primitivesEV[] = { { PTS, "Point" }, { WIREFRAME, "Wireframe" }, {
			SOLID, "Solid" } };
	TwType primtiveType = TwDefineEnum("PrimitiveType", primitivesEV, 3);
	TwAddVarRW(operbar, "Primitives", primtiveType, &g_scene_.prim, NULL);
	TwType shadingType;
	shadingType = TwDefineEnum("shadingType", NULL, 0);
	TwAddVarRW(operbar, "Shading", shadingType, &g_scene_.shading,
			" enum='0{Flat},1{Smooth}' ");

	TwAddSeparator(operbar, "", "");
	TwAddVarRW(operbar, "Rot X1", TW_TYPE_FLOAT, &g_scene_.rotation_.rot1[0],
			"group='Joint DOF' min=-360 max=360 step=5");
	TwAddVarRW(operbar, "Rot Y1", TW_TYPE_FLOAT, &g_scene_.rotation_.rot1[1],
			"group='Joint DOF' min=-360 max=360 step=5");
	TwAddVarRW(operbar, "Rot Z1", TW_TYPE_FLOAT, &g_scene_.rotation_.rot1[2],
			"group='Joint DOF' min=-360 max=360 step=5");

	TwAddVarRW(operbar, "Rot X2", TW_TYPE_FLOAT, &g_scene_.rotation_.rot2[0],
			"group='Joint DOF' min=-360 max=360 step=5");
	TwAddVarRW(operbar, "Rot Y2", TW_TYPE_FLOAT, &g_scene_.rotation_.rot2[1],
			"group='Joint DOF' min=-360 max=360 step=5");
	TwAddVarRW(operbar, "Rot Z2", TW_TYPE_FLOAT, &g_scene_.rotation_.rot2[2],
			"group='Joint DOF' min=-360 max=360 step=5");

	TwAddVarRW(operbar, "Rot X3", TW_TYPE_FLOAT, &g_scene_.rotation_.rot3[0],
			"group='Joint DOF' min=-360 max=360 step=5");
	TwAddVarRW(operbar, "Rot Y3", TW_TYPE_FLOAT, &g_scene_.rotation_.rot3[1],
			"group='Joint DOF' min=-360 max=360 step=5");
	TwAddVarRW(operbar, "Rot Z3", TW_TYPE_FLOAT, &g_scene_.rotation_.rot3[2],
			"group='Joint DOF' min=-360 max=360 step=5");
	TwAddVarRW(operbar, "Reset Linkage", TW_TYPE_BOOL8,
			&g_scene_.rotation_.reset_, " help='reset to the initial status'");
	TwAddVarRW(operbar, "Trans X", TW_TYPE_FLOAT, &g_scene_.camera_.trans[0],
			"group='Camera Transfromation' min=-20 max=20 step=0.5");
	TwAddVarRW(operbar, "Trans Y", TW_TYPE_FLOAT, &g_scene_.camera_.trans[1],
			"group='Camera Transfromation' min=-20 max=20 step=0.5");
	TwAddVarRW(operbar, "Trans Z", TW_TYPE_FLOAT, &g_scene_.camera_.trans[2],
			"group='Camera Transfromation' min=-50 max=50 step=0.5");

	TwAddSeparator(operbar, "", "");
	TwAddVarRW(operbar, "Target End X", TW_TYPE_FLOAT,
			&g_scene_.rotation_.targetEndEffect[0],
			"group='Target End Effect' min=-20 max=20 step=0.5");
	TwAddVarRW(operbar, "Target End Y", TW_TYPE_FLOAT,
			&g_scene_.rotation_.targetEndEffect[1],
			"group='Target End Effect' min=-20 max=20 step=0.5");
	TwAddVarRW(operbar, "Target End Z", TW_TYPE_FLOAT,
			&g_scene_.rotation_.targetEndEffect[2],
			"group='Target End Effect' min=-50 max=50 step=0.5");
	TwAddSeparator(operbar, "", "");
	TwAddVarRW(operbar, "Rot X", TW_TYPE_FLOAT, &g_scene_.camera_.rot[0],
			"group='Camera Transfromation' min=-360 max=360 step=5");
	TwAddVarRW(operbar, "Rot Y", TW_TYPE_FLOAT, &g_scene_.camera_.rot[1],
			"group='Camera Transfromation' min=-360 max=360 step=5");
	TwAddVarRW(operbar, "Rot Z", TW_TYPE_FLOAT, &g_scene_.camera_.rot[2],
			"group='Camera Transfromation' min=-360 max=360 step=5");

	TwAddVarRW(operbar, "Trans X", TW_TYPE_FLOAT, &g_scene_.camera_.trans[0],
			"group='Camera Transfromation' min=-20 max=20 step=0.5");
	TwAddVarRW(operbar, "Trans Y", TW_TYPE_FLOAT, &g_scene_.camera_.trans[1],
			"group='Camera Transfromation' min=-20 max=20 step=0.5");
	TwAddVarRW(operbar, "Trans Z", TW_TYPE_FLOAT, &g_scene_.camera_.trans[2],
			"group='Camera Transfromation' min=-50 max=50 step=0.5");
	TwAddSeparator(operbar, "", "");

	TwAddVarRW(operbar, "Reset", TW_TYPE_BOOL8, &g_scene_.reset_,
			" help='reset to the initial status'");
	TwAddSeparator(operbar, "", "");
	TwAddVarRW(operbar, "LightON", TW_TYPE_BOOL8, &g_scene_.lightson, "");
	TwAddVarRW(operbar, "Ambient1", TW_TYPE_COLOR4F, &g_scene_.light1.ambient,
			"group=light1");
	TwAddVarRW(operbar, "Diffuse1", TW_TYPE_COLOR4F, &g_scene_.light1.diffuse,
			"group=light1");
	TwAddVarRW(operbar, "Specular1", TW_TYPE_COLOR4F, &g_scene_.light1.specular,
			"group=light1");
	TwAddVarRW(operbar, "Ambient2", TW_TYPE_COLOR4F, &g_scene_.light2.ambient,
			"group=light2");
	TwAddVarRW(operbar, "Diffuse2", TW_TYPE_COLOR4F, &g_scene_.light2.diffuse,
			"group=light2");
	TwAddVarRW(operbar, "Specular2", TW_TYPE_COLOR4F, &g_scene_.light2.specular,
			"group=light2");
}

GLvoid Timer(int value) {
	++g_scene_.animate_.currenttime_;
	g_scene_.animate_.currenttime_ %= g_scene_.animate_.maxtime_;
	if (value)
		glutPostRedisplay();
	glutTimerFunc(1200 / g_scene_.animate_.fps, Timer, value);
}
int main(int argc, char ** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	//glutInitContextVersion(3, 3);
	//glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Animate");
	glutCreateMenu(NULL);
	//TwInit(TW_OPENGL_CORE, NULL);
	TwInit(TW_OPENGL, NULL);

//	//****The following part is used to test whether your compute support opengl 3.3//
//	glewExperimental = GL_TRUE;
//	if(glewInit()){
//		std::cerr << "Unable to initialize GLEW ...exiting" << std::endl;
//		exit(EXIT_FAILURE);
//	}
//
//	if (glewIsSupported("GL_VERSION_3_3"))
//		std::cout <<"Ready for OpenGL 3.3\n";
//	else {
//		std::cout <<"OpenGL 3.3 not supported\n";
//		exit(1);
//	}
//	//*****//

	// Set GLUT event callbacks
	// - Directly redirect GLUT mouse button events to AntTweakBar
	glutMouseFunc((GLUTmousebuttonfun) TwEventMouseButtonGLUT);
	// - Directly redirect GLUT mouse motion events to AntTweakBar
	glutMotionFunc((GLUTmousemotionfun) TwEventMouseMotionGLUT);
	// - Directly redirect GLUT mouse "passive" motion events to AntTweakBar (same as MouseMotion)
	glutPassiveMotionFunc((GLUTmousemotionfun) TwEventMouseMotionGLUT);
	// - Directly redirect GLUT key events to AntTweakBar
	glutKeyboardFunc((GLUTkeyboardfun) TwEventKeyboardGLUT);

	glutTimerFunc(1200 / g_scene_.animate_.fps, Timer, 0);

	// - Directly redirect GLUT special key events to AntTweakBar
	glutSpecialFunc((GLUTspecialfun) TwEventSpecialGLUT);
	// - Send 'glutGetModifers' function pointer to AntTweakBar;
	//   required because the GLUT key event functions do not report key modifiers states.
	TwGLUTModifiersFunc(glutGetModifiers);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	init();
	init_gui();

	glutMainLoop();
	atexit(Terminate);
	return 0;
}
