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

typedef enum{PTS, WIREFRAME, SOLID} primitive;
typedef enum{FLAT, SMOOTH} shademode;

//**** animation code ****// You need to further implement the animation class to support the rotating cube
class animation{
public:
	int currenttime = 1;
	const int maxtime = 1200;
	const float fps = 30.0;
	void init() {
		currenttime = 1;
		pos_ = vmath::vec3(5.0, 0.0, 0.0);
	}
	vmath::vec3 pos_;
	void reset() {
		init();
	}

	void update() {}
};

class color{
public:
	vmath::vec4 ambient;
	vmath::vec4 diffuse;
	vmath::vec4 specular;
	float shine;
	color(){
		ambient = vmath::vec4(0,0,0,1);
		diffuse = vmath::vec4(0,0.5,0,1);
		specular = vmath::vec4(0,0,0.5,1);
		shine = 1.0;
	}
};

class model{
public:
	const int trinum_ = 12;
	GLfloat cube_[108] = {-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		    -1.0f,-1.0f, 1.0f,
		    -1.0f, 1.0f, 1.0f, // triangle 1 : end
		    1.0f, 1.0f,-1.0f, // triangle 2 : begin
		    -1.0f,-1.0f,-1.0f,
		    -1.0f, 1.0f,-1.0f, // triangle 2 : end
		    1.0f,-1.0f, 1.0f,
		    -1.0f,-1.0f,-1.0f,
		    1.0f,-1.0f,-1.0f,
		    1.0f, 1.0f,-1.0f,
		    1.0f,-1.0f,-1.0f,
		    -1.0f,-1.0f,-1.0f,
		    -1.0f,-1.0f,-1.0f,
		    -1.0f, 1.0f, 1.0f,
		    -1.0f, 1.0f,-1.0f,
		    1.0f,-1.0f, 1.0f,
		    -1.0f,-1.0f, 1.0f,
		    -1.0f,-1.0f,-1.0f,
		    -1.0f, 1.0f, 1.0f,
		    -1.0f,-1.0f, 1.0f,
		    1.0f,-1.0f, 1.0f,
		    1.0f, 1.0f, 1.0f,
		    1.0f,-1.0f,-1.0f,
		    1.0f, 1.0f,-1.0f,
		    1.0f,-1.0f,-1.0f,
		    1.0f, 1.0f, 1.0f,
		    1.0f,-1.0f, 1.0f,
		    1.0f, 1.0f, 1.0f,
		    1.0f, 1.0f,-1.0f,
		    -1.0f, 1.0f,-1.0f,
		    1.0f, 1.0f, 1.0f,
		    -1.0f, 1.0f,-1.0f,
		    -1.0f, 1.0f, 1.0f,
		    1.0f, 1.0f, 1.0f,
		    -1.0f, 1.0f, 1.0f,
		    1.0f,-1.0f, 1.0f};
	GLfloat normal_[108] = {
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
	};
	const int planetrinum_ = 2;
	GLfloat plane_[18] = {
			1.0f, 0.0f, 1.0f,
			1.0f, 0.0f,-1.0f,
			-1.0f, 0.0f,-1.0f,
			1.0f, 0.0f, 1.0f,
			-1.0f, 0.0f,-1.0f,
			-1.0f, 0.0f, 1.0f};
	GLfloat pnormal_[18] = {
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f};
	GLfloat axis_[54] = {-1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						0.75f, 0.25f, 0.0f,
						1.0f, 0.0f, 0.0f,
						0.75f, -0.25f, 0.0f,
						0.0f, -1.0f, 0.0f,
						0.0f, 1.0f, 0.0f,
						0.0f, 1.0f, 0.0f,
						0.25f, 0.75f, 0.0f,
						0.0f, 1.0f, 0.0f,
						-0.25f, 0.75f, 0.0f,
						0.0f, 0.0f, -1.0f,
						0.0f, 0.0f, 1.0f,
						0.0f, 0.0f, 1.0f,
						0.0f, 0.25f, 0.75f,
						0.0f, 0.0f, 1.0f,
						0.0f, -0.25f, 0.75f};
	GLfloat anormal_[54] = {1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f};
	vmath::vec4 axismat[3] = {{1.0f, 0.0f,0.0f, 0.0f},{0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0}};
};
class parameters{
public:
	float a;
	float b;
	void init() {
			a = 0.0f;
				b = 0.0f;
				reset();
			}
	void reset(){
				a = 0.0f;
				b = 0.0f;

			}
};
class camera{
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
	void reset(){
		rot = vmath::vec3(0,0,0);
		trans = vmath::vec3(0,0,0);
		u = vmath::vec3(1,0,0);
		v = vmath::vec3(0,1,0);
		w = vmath::vec3(0,0,1);
		eye = vmath::vec3(0,0,15);

		lastrot = vmath::vec3(0.0,0.0,0.0);
		lasttrans = vmath::vec3(0.0,0.0,0.0);
		M = vmath::mat4::identity();
	}
	void update() {
		if(rot[0] != lastrot[0] || rot[1] != lastrot[1] || rot[2] != lastrot[2]){
			vmath::mat4 rt = vmath::mat4::identity();
			if(rot[0] != lastrot[0]){
				rt = vmath::rotate(-rot[0] + lastrot[0], vmath::vec3(1,0,0));
			}else if(rot[1] != lastrot[1]){
				rt = vmath::rotate(-rot[1] + lastrot[1], vmath::vec3(0,1,0));
			}else if(rot[2]  != lastrot[2]){
				rt = vmath::rotate(-rot[2] + lastrot[2], vmath::vec3(0,0,1));
			}
			M = rt * M;
			lastrot = rot;
		}
		if(trans[0] != lasttrans[0] || trans[1] != lasttrans[1] || trans[2] != lasttrans[2]){
			vmath::vec3 tt = vmath::vec3(0,0,0);
			if(trans[0] != lasttrans[0]){
				tt = -(trans[0] - lasttrans[0]) * u;
			}else if(trans[1] != lasttrans[1]){
				tt = -(trans[1] - lasttrans[1]) * v;
			}else if(trans[2] != lasttrans[2]){
				tt = -(trans[2] - lasttrans[2]) * w;
			}
			vmath::mat4 mt = vmath::translate(tt);
			M = mt * M;
			//eye = vmath::vec3(M[3][0], M[3][1], M[3][1] + 15);
			lasttrans = trans;
		}
	}
};
class scene{
public:
	bool cw;
	primitive prim;
	shademode shading;
	bool reset_;
	camera camera_;
	animation animate_;
	parameters parameters_;

// light para
	bool lightson;
	color light1;
	color light2;

	scene(){
		init();
	}
	void init(){
		cw = false; // using GL_CW
		prim = SOLID;
		shading = FLAT;
		reset_= false;
		camera_.init();
		animate_.init();
		parameters_.init();
		reset();
	}
	void reset(){
		camera_.reset();

		animate_.reset();


		parameters_.init();
		lightson = true;
		light1.ambient = vmath::vec4(0.5,0.5,0.5,1.0);
		light1.diffuse = vmath::vec4(0.5,0.5,0.5,1.0);
		light1.specular = vmath::vec4(0.1,0.1,0.1,1.0);
		light2.ambient = vmath::vec4(0.5,0.5,0.5,1.0);
		light2.diffuse = vmath::vec4(0.5,0.5,0.5,1.0);
		light2.specular = vmath::vec4(0.1,0.1,0.1,1.0);
	}
};


//shader shader_;
model gmodel_;
scene g_scene_;

void TW_CALL CopyStdStringToClient(std::string & dst, const std::string & src){
	dst = src;
}

void reshape(int width, int height){
	//GLfloat aspect = (GLfloat) width / (GLfloat) height;
	glViewport(0, 0, width, height);
	TwWindowSize(width, height);
}

void setmaterial_old(std::string whichone) {
	vmath::vec4 ambient, diffuse, specular;
	GLfloat shine = 0.5;
	if(whichone == "cube") {
		ambient = vmath::vec4(0.4, 0.0, 0.0, 1.0);
		diffuse = vmath::vec4(0.6, 0.0, 0.0, 1.0);
		specular = vmath::vec4(0.0, 0.0, 0.0, 1.0);
	}else if(whichone == "yaxis") {
		ambient = vmath::vec4(0.0, 0.4, 0.0, 1.0);
		diffuse = vmath::vec4(0.0, 0.6, 0.0, 1.0);
		specular = vmath::vec4(0.0, 0.0, 0.0, 1.0);
	}else if(whichone == "axis") {
		ambient = vmath::vec4(0.0, 0.0, 0.5, 1.0);
		diffuse = vmath::vec4(0.0, 0.0, 0.5, 1.0);
		specular = vmath::vec4(0.0, 0.0, 0.0, 1.0);
	}else{
		ambient = vmath::vec4(0.5, 0.5, 0.5, 1.0);
		diffuse = vmath::vec4(0.5, 0.5, 0.5, 1.0);
		specular = vmath::vec4(0.0, 0.0, 0.0, 1.0);
	}
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shine);
}

void lightsInitialize(){
  GLfloat light0_ambient[]  = {0.5, 0.5, 0.5, 1.0 };
  GLfloat light0_diffuse[]  = {0.5, 0.5, 0.5, 1.0 };
  GLfloat light0_specular[] = {0.1, 0.1, 0.1, 1.0 };

  GLfloat light1_ambient[]  = {0.5, 0.5, 0.5, 1.0 };
  GLfloat light1_diffuse[]  = {0.5, 0.5, 0.5, 1.0 };
  GLfloat light1_specular[] = {0.1, 0.1, 0.1, 1.0 };


  glEnable(GL_LIGHTING);

  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR,light0_specular);

  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR,light1_specular);
}

void lightsPosition(){
  GLfloat light0_position[] = { 0.0, 10.0, 3.0, 1.0 };
  GLfloat light1_position[] = { -5.0,-10.0,2.0,1.0 };

  glLightfv(GL_LIGHT0, GL_POSITION,light0_position);
  glLightfv(GL_LIGHT1, GL_POSITION,light1_position);
}

void display_old() {
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	glDepthFunc(GL_LEQUAL);

	switch(g_scene_.prim){
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
		std::cout<< "error prim type!\n";
	}

	if(g_scene_.cw){
		glFrontFace(GL_CW);
	}else{
		glFrontFace(GL_CCW);
	}
	if(g_scene_.reset_){
		g_scene_.reset();
		g_scene_.reset_ = false;
	}

	switch(g_scene_.shading) {
		case FLAT:
			glShadeModel(GL_FLAT);
			break;
		case SMOOTH:
			glShadeModel(GL_SMOOTH);
			break;
		default:
			std::cout << "error shading mode\n";
	}
	if(g_scene_.lightson) {
		lightsInitialize();
		lightsPosition();
	}else{
		glDisable(GL_LIGHTING);
		glColor3f(0.4f, 0.0f, 0.6f);
	}
	//projection matrix
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	vmath::mat4 projectionmat = vmath::perspective(60,(double)width / (double)height,g_scene_.camera_.near_, g_scene_.camera_.far_);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (double) width / (double) height, g_scene_.camera_.near_, g_scene_.camera_.far_);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	g_scene_.camera_.update();
	glLoadIdentity();
	vmath::mat4 Mlookat = vmath::lookat(g_scene_.camera_.eye, g_scene_.camera_.eye - vmath::vec3(0,0,1), vmath::vec3(0,1,0));
	Mlookat = g_scene_.camera_.M * Mlookat;
	GLfloat m[16];
	for(int i = 0; i < 4; ++i){
		for(int j = 0; j < 4; ++j) {
			m[4 * i + j] = Mlookat[i][j];
		}
	}
	glMultMatrixf(m);

	//draw cube
	glPushMatrix();
	g_scene_.animate_.update();
	glRotatef(g_scene_.parameters_.b, 0.0f, 1.0f, 0.0f);
	glTranslatef(g_scene_.animate_.pos_[0], g_scene_.animate_.pos_[1], g_scene_.animate_.pos_[2]);

	setmaterial_old("cube");
	glRotatef(g_scene_.parameters_.a, 0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < gmodel_.trinum_ * 3; ++i) {
		glVertex3f(gmodel_.cube_[3 * i], gmodel_.cube_[3 * i + 1], gmodel_.cube_[3 * i + 2]);
		glNormal3f(gmodel_.normal_[3 * i], gmodel_.normal_[3 * i + 1], gmodel_.normal_[3 * i + 2]);
	}
	glEnd();


	// draw local axis;
	glScalef(2.0f, 2.0f, 2.0f);
	glBegin(GL_LINES);
	for(int i = 0; i < 3; ++i){
		glMaterialfv(GL_FRONT, GL_AMBIENT, gmodel_.axismat[i]);
		for(int j = 0; j < 6; ++j) {
			glVertex3f(gmodel_.axis_[i * 18 + j * 3 ], gmodel_.axis_[i * 18 + j * 3 + 1], gmodel_.axis_[i * 18 + j * 3 + 2]);
			glNormal3f(gmodel_.anormal_[i * 18 + j * 3], gmodel_.anormal_[i * 18 + j * 3 + 1], gmodel_.anormal_[i * 18 + j * 3 + 2]);
		}
	}
	glEnd();
	glRotatef(-g_scene_.parameters_.a, 0.0f, 0.0f, 1.0f);
	glTranslatef(-g_scene_.animate_.pos_[0],-g_scene_.animate_.pos_[1],-g_scene_.animate_.pos_[2]);
	glRotatef(-g_scene_.parameters_.b, 0.0f, 1.0f, 0.0f);

	glPopMatrix();
	//draw global yaxis;
	glPushMatrix();
	glScalef(0.5f, 10.0f, 0.5f);
	setmaterial_old("yaxis");
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < gmodel_.trinum_ * 3; ++i) {
		glVertex3f(gmodel_.cube_[3 * i], gmodel_.cube_[3 * i + 1], gmodel_.cube_[3 * i + 2]);
		glNormal3f(gmodel_.normal_[3 * i], gmodel_.normal_[3 * i + 1], gmodel_.normal_[3 * i + 2]);
	}
	glEnd();
	glPopMatrix();

	// draw plane;
	glPushMatrix();
	setmaterial_old("other");
	glTranslatef(0.0f, -2.0f, 0.0f);
	glScalef(8.0f, 8.0f, 8.0f);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < gmodel_.planetrinum_ * 3; ++i) {
		glVertex3f(gmodel_.plane_[3 * i], gmodel_.plane_[3 * i + 1], gmodel_.plane_[3 * i + 2]);
		glNormal3f(gmodel_.pnormal_[3 * i], gmodel_.pnormal_[3 * i + 1], gmodel_.pnormal_[3 * i + 2]);
	}
	glEnd();
	glPopMatrix();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	TwDraw();
	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}

void Terminate(void){
	TwTerminate();
}
void reset(){
	g_scene_.reset();
}

void init_gui(){
	TwBar *operbar = TwNewBar("ControlBar");
	TwDefine(" GLOBAL help='CubeAnimation' ");
	TwDefine(" ControlBar position='16 16' size='200 300' color='96 216 224' ");
	TwAddVarRW(operbar,"CW/CCW", TW_TYPE_BOOL8, &g_scene_.cw, NULL);
	TwEnumVal primitivesEV[] = {{PTS,"Point"},{WIREFRAME,"Wireframe"},{SOLID,"Solid"}};
	TwType primtiveType = TwDefineEnum("PrimitiveType",primitivesEV,3);
	TwAddVarRW(operbar,"Primitives", primtiveType, &g_scene_.prim, NULL);
	TwType shadingType;
	shadingType = TwDefineEnum("shadingType", NULL, 0);
	TwAddVarRW(operbar, "Shading", shadingType, &g_scene_.shading, " enum='0{Flat},1{Smooth}' ");


	TwAddSeparator(operbar,"","");
	TwAddVarRW(operbar, "a", TW_TYPE_FLOAT, &g_scene_.parameters_.a, "group='Parameters' min=-360 max=360 step=5");
	TwAddVarRW(operbar, "b", TW_TYPE_FLOAT, &g_scene_.parameters_.b, "group='Parameters' min=-360 max=360 step=5");

	TwAddSeparator(operbar, "","");

		TwAddVarRW(operbar, "Rot X", TW_TYPE_FLOAT, &g_scene_.camera_.rot[0], "group='Camera Transfromation' min=-360 max=360 step=5");
		TwAddVarRW(operbar, "Rot Y", TW_TYPE_FLOAT, &g_scene_.camera_.rot[1], "group='Camera Transfromation' min=-360 max=360 step=5");
		TwAddVarRW(operbar, "Rot Z", TW_TYPE_FLOAT, &g_scene_.camera_.rot[2], "group='Camera Transfromation' min=-360 max=360 step=5");
		TwAddSeparator(operbar, "", "");
		TwAddVarRW(operbar, "Trans X", TW_TYPE_FLOAT, &g_scene_.camera_.trans[0], "group='Camera Transfromation' min=-20 max=20 step=0.5");
		TwAddVarRW(operbar, "Trans Y", TW_TYPE_FLOAT, &g_scene_.camera_.trans[1], "group='Camera Transfromation' min=-20 max=20 step=0.5");
		TwAddVarRW(operbar, "Trans Z", TW_TYPE_FLOAT, &g_scene_.camera_.trans[2], "group='Camera Transfromation' min=-50 max=50 step=0.5");

	TwAddVarRW(operbar, "Reset", TW_TYPE_BOOL8, &g_scene_.reset_," help='reset to the initial status'");
	TwAddSeparator(operbar, "","");
	TwAddVarRW(operbar, "LightON", TW_TYPE_BOOL8, &g_scene_.lightson, "");
	TwAddVarRW(operbar, "Ambient1", TW_TYPE_COLOR4F, &g_scene_.light1.ambient, "group=light1");
	TwAddVarRW(operbar, "Diffuse1", TW_TYPE_COLOR4F, &g_scene_.light1.diffuse, "group=light1");
	TwAddVarRW(operbar, "Specular1", TW_TYPE_COLOR4F, &g_scene_.light1.specular, "group=light1");
	TwAddVarRW(operbar, "Ambient2", TW_TYPE_COLOR4F, &g_scene_.light2.ambient, "group=light2");
	TwAddVarRW(operbar, "Diffuse2", TW_TYPE_COLOR4F, &g_scene_.light2.diffuse, "group=light2");
	TwAddVarRW(operbar, "Specular2", TW_TYPE_COLOR4F, &g_scene_.light2.specular, "group=light2");
}

GLvoid Timer(int value) {
	++g_scene_.animate_.currenttime;
	g_scene_.animate_.currenttime %= g_scene_.animate_.maxtime;
	if( value )
		glutPostRedisplay();
	glutTimerFunc(1200/g_scene_.animate_.fps, Timer, value);
}

int main(int argc, char ** argv){
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
	glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	// - Directly redirect GLUT mouse motion events to AntTweakBar
	glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	// - Directly redirect GLUT mouse "passive" motion events to AntTweakBar (same as MouseMotion)
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	// - Directly redirect GLUT key events to AntTweakBar
	glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);

	glutTimerFunc(1200 / g_scene_.animate_.fps, Timer, 0);

	// - Directly redirect GLUT special key events to AntTweakBar
	glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
	// - Send 'glutGetModifers' function pointer to AntTweakBar;
	//   required because the GLUT key event functions do not report key modifiers states.
	TwGLUTModifiersFunc(glutGetModifiers);

	glutDisplayFunc(display_old);
	glutReshapeFunc(reshape);

	init_gui();

	glutMainLoop();
	atexit(Terminate);
	return 0;
}
