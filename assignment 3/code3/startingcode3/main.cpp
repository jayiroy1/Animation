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

//**** animation code ****// You need to further implement the animation class to support the rotating cube
class animation {
public:
	int currenttime_ = 1;
	const int maxtime_ = 1600;
	const float fps = 30.0;
	const int pnum_ = 8;
	float t1=0.5,t2=0.50;
	vmath::vec3 points_[8] = { { 0.0f, 1.5f, -2.0f }, { -3.0f, 4.0f, 2.3f }, { -6.0f,
			1.5f, -2.5f }, { -4.0f, -1.5f, 2.8f }, { 1.0f, -5.0f, -4.0f }, { 4.0f, -1.0f,
			3.0f }, { 7.0f, 1.0f, -2.0f }, { 3.0f, 3.0f, 3.7f} };
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
	std::vector<vmath::vec3> cube_ = { { -1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f,
			1.0f }, { -1.0f, 1.0f, -1.0f }, { -1.0f, 1.0f, 1.0f }, { 1.0f,
			-1.0f, -1.0f }, { 1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f, -1.0f }, {
			1.0f, 1.0f, 1.0f } };
	std::vector<std::array<int, 3>> cubefaces_ = { { 0, 6, 4 }, { 0, 2, 6 }, {
			0, 3, 2 }, { 0, 1, 3 }, { 2, 7, 6 }, { 2, 3, 7 }, { 4, 6, 7 }, { 4,
			7, 5 }, { 0, 4, 5 }, { 0, 5, 1 }, { 1, 5, 7 }, { 1, 7, 3 } };
	std::vector<vmath::vec3> cnormals_;
	vmath::vec3 cubescale_ = vmath::vec3(0.3f, 0.3f, 0.3f);
	vmath::vec3 planescale_ = vmath::vec3(6.0f, 6.0f, 6.0f);
	vmath::vec3 modelscale_ = vmath::vec3(1.0f, 1.0f, 1.0f);
	vmath::vec3 planetrans_ = vmath::vec3(0.0f, -2.0f, 0.0f);
	vmath::vec3 modelcenter_;
	const int planetrinum_ = 2;
	std::vector<vmath::vec3> plane_ = { { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f,
			-1.0f }, { -1.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 1.0f }, { -1.0f,
			0.0f, -1.0f }, { -1.0f, 0.0f, 1.0f } };
	std::vector<GLfloat> pnormals_ = { 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f };
	std::vector<vmath::vec3> axis_ = { { -1.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.75f, 0.25f, 0.0f },
			{ 1.0f, 0.0f, 0.0f }, { 0.75f, -0.25f, 0.0f },
			{ 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, {
					0.25f, 0.75f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { -0.25f, 0.75f,
					0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f,
					0.0f, -1.0f }, { 0.0f, 0.25f, -0.75f },
			{ 0.0f, 0.0f, -1.0f }, { 0.0f, -0.25f, -0.75f } };
	vmath::vec4 axismat[3] = { { 1.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f,
			0.0f }, { 0.0f, 0.0f, 1.0f, 0.0 } };
	std::vector<vmath::vec3> vertices_;
	std::vector<std::array<int, 3>> triangles_;
	std::vector<vmath::vec3> normals_;
	void load_obj(std::string& filename);
	void init() {
		cnormals_.resize(cubefaces_.size());
		for (unsigned i = 0; i < cubefaces_.size(); ++i) {
			vmath::vec3 ab = cube_[cubefaces_[i][1]] - cube_[cubefaces_[i][0]];
			vmath::vec3 ac = cube_[cubefaces_[i][2]] - cube_[cubefaces_[i][0]];
			cnormals_[i] = normalize(cross(ab, ac));
		}
	}
};

void model::load_obj(std::string& filename) {
	std::ifstream in(filename);
	if (in.fail()) {
		std::cout << filename << std::endl;
		throw std::logic_error(
				"Can't read the mesh filea at BaseMode::ReadObjFile().");
	}
	char buf[256];
	vmath::vec3 pt=vmath::vec3(0.0f,0.0f,0.0f);
	std::array<int, 3> ft;
	vmath::vec3 bmin(INFINITY, INFINITY, INFINITY), bmax(-INFINITY, -INFINITY,
			-INFINITY);
	while (in.getline(buf, sizeof buf)) {
		std::istringstream line(buf);
		std::string word;
		line >> word;
		if (word == "v") {
			line >> pt[0] >> pt[1] >> pt[2];
			vertices_.push_back(pt);
			for (int k = 0; k < 3; ++k) {
				bmin[k] = std::min(pt[k], bmin[k]);
				bmax[k] = std::max(pt[k], bmax[k]);
			}
		} else if (word == "f") {
			char ch,cha;
			int tex;
	
			line >> ft[0]>>ch>>tex>>ch>>tex>> ft[1] >> ch >> tex >> ch >> tex >> ft[2] >> ch >> tex >> ch >> tex ;
			/*line >> p >> ch >> ch >> q >>
				r >> ch >> ch >> s >> t >> ch >> ch >> u;
			ft[0] = p / q;
			ft[1] = r / s;
			ft[2] = t / u;*/
			for (unsigned i = 0; i < 3; i++) {
				--ft[i];
			}
			triangles_.push_back(ft);
		}
	}
	modelcenter_ = (bmin + bmax);
	modelcenter_ *= 0.5;
	vmath::vec3 diff = bmax - bmin;
	float scale = 1.2 / std::max(std::max(diff[0], diff[1]), diff[2]);
	modelscale_ = vmath::vec3(scale, scale, scale);
	in.close();
	normals_.resize(triangles_.size());
	vmath::vec3 aee = vertices_[triangles_[1][1]];
	for (unsigned i = 0; i < triangles_.size(); ++i) {
		vmath::vec3 ab = vertices_[triangles_[i][1]]
				- vertices_[triangles_[i][0]];
		vmath::vec3 ac = vertices_[triangles_[i][2]]
				- vertices_[triangles_[i][0]];
		normals_[i] = normalize(cross(ab, ac));
		std::cout << i << "\n";
	}
}

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
class scene {
public:
	bool cw;
	primitive prim;
	shademode shading;

	bool reset_;
	camera camera_;
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
		animate_.init();
		reset();
	}
	void reset() {
		camera_.reset();

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

float table[1601][6];
int shuttleTime=0.0;
const int pnum_ = 8;


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
		ambient = vmath::vec4(0.4, 0.0, 0.0, 1.0);
		diffuse = vmath::vec4(0.6, 0.0, 0.0, 1.0);
		specular = vmath::vec4(0.0, 0.0, 0.0, 1.0);
	} else if (whichone == "model") {
		ambient = vmath::vec4(0.0, 0.4, 0.0, 1.0);
		diffuse = vmath::vec4(0.0, 0.6, 0.0, 1.0);
		specular = vmath::vec4(0.0, 0.0, 0.0, 1.0);
	} else if (whichone == "axis") {
		ambient = vmath::vec4(0.0, 0.0, 0.5, 1.0);
		diffuse = vmath::vec4(0.0, 0.0, 0.5, 1.0);
		specular = vmath::vec4(0.0, 0.0, 0.0, 1.0);
	} else if (whichone == "plane") {
		ambient = vmath::vec4(0.5, 0.5, 0.5, 1.0);
		diffuse = vmath::vec4(0.5, 0.5, 0.5, 1.0);
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

void lightsPosition() {
	GLfloat light0_position[] = { 0.0, 10.0, 3.0, 1.0 };
	GLfloat light1_position[] = { -5.0, -10.0, 2.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
}

class point {
public:
	float z;
	float x;
	float y;

	point(float X, float Y, float Z) {
		x = X;
		y = Y;
		z = Z;

	}
	point() {
		x = y = z = 0.00;

	}

};

point CatmullRoll(float t, struct point p1, struct point p2, struct point p3,
		struct point p4) {

	float t2 = t * t;
	float t3 = t * t * t;
	point v; // Interpolated point

	/* Catmull Rom spline Calculation */

	v.x = ((-t3 + 2 * t2 - t) * (p1.x) + (3 * t3 - 5 * t2 + 2) * (p2.x)
			+ (-3 * t3 + 4 * t2 + t) * (p3.x) + (t3 - t2) * (p4.x)) / 2;
	v.y = ((-t3 + 2 * t2 - t) * (p1.y) + (3 * t3 - 5 * t2 + 2) * (p2.y)
			+ (-3 * t3 + 4 * t2 + t) * (p3.y) + (t3 - t2) * (p4.y)) / 2;
	v.z = ((-t3 + 2 * t2 - t) * (p1.z) + (3 * t3 - 5 * t2 + 2) * (p2.z)
			+ (-3 * t3 + 4 * t2 + t) * (p3.z) + (t3 - t2) * (p4.z)) / 2;

	return v;
}

float time2dis(float time){

	if (time<=g_scene_.animate_.t1){
				return (time*time*(1/(g_scene_.animate_.t1*(g_scene_.animate_.t2-g_scene_.animate_.t1+1))));
			}
			else if (g_scene_.animate_.t1<time && time<g_scene_.animate_.t2){
				return (g_scene_.animate_.t1*g_scene_.animate_.t1*(1/(g_scene_.animate_.t1*(g_scene_.animate_.t2-g_scene_.animate_.t1+1))) + (time-g_scene_.animate_.t1)*g_scene_.animate_.t1*(2/(g_scene_.animate_.t1*(g_scene_.animate_.t2-g_scene_.animate_.t1+1))));

			}
			else if (g_scene_.animate_.t2<=time && time<=1){

				return ( 1-((1-time)*(1-time))*1/((1-g_scene_.animate_.t2)*(g_scene_.animate_.t2-g_scene_.animate_.t1+1)));
			}

}

float (&dis2u(float s))[2]{
	float u=0;
	float section=0;
	float ans[2];
	for (int i=0;i<1600;i++){
		if(table[i][2]<s && s<table [i+1][2]){
			u= (table[i][1]+(((table[i+1][1]-table[i][1])*(table[i][2]-s))/(table[i+1][1]-table[i][1])));
			section = table[i][0];
		}
	}

	ans[0]=section;
	ans[1]=u;
	return ans;
}

point u2p(float k, float t){
	int i=(int) k;

	point p1(g_scene_.animate_.points_[i][0],
							g_scene_.animate_.points_[i][1],
							g_scene_.animate_.points_[i][2]), p2(
							g_scene_.animate_.points_[(i + 1) % 8][0],
							g_scene_.animate_.points_[(i + 1) % 8][1],
							g_scene_.animate_.points_[(i + 1) % 8][2]), p3(
							g_scene_.animate_.points_[(i + 2) % 8][0],
							g_scene_.animate_.points_[(i + 2) % 8][1],
							g_scene_.animate_.points_[(i + 2) % 8][2]), p4(
							g_scene_.animate_.points_[(i + 3) % 8][0],
							g_scene_.animate_.points_[(i + 3) % 8][1],
							g_scene_.animate_.points_[(i + 3) % 8][2]);
			//glColor3f(0.0f,0.0f,0.0f);
//glColor3f(0.0f,0.0f,0.0f);

	return CatmullRoll(t, p1, p2, p3, p4);


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

	//draw model;
	glPushMatrix();

	//glScalef(gmodel_.cubescale_[0], gmodel_.cubescale_[1],
			//gmodel_.cubescale_[2]);
	//glTranslatef(table[g_scene_.animate_.currenttime_][3],table[g_scene_.animate_.currenttime_][4],table[g_scene_.animate_.currenttime_][5]);
	float shuttleTimef=((float)g_scene_.animate_.currenttime_)/((float)1600);
	float dis =time2dis(shuttleTimef);
float u[2];
	u[0]=dis2u(dis)[0];
	u[1]=dis2u(dis)[1];
	float a=1;
	float b=2;
	point present=u2p(u[0],u[1]); //= new point(1,2,3);//u2p(1,2);
	glTranslatef(present.x,present.y,present.z);

//glScalef(gmodel_.modelscale_[0], gmodel_.modelscale_[1],
		//	gmodel_.modelscale_[2]);

	glBegin(GL_TRIANGLES);
	for (unsigned i = 0; i < gmodel_.triangles_.size(); ++i) {
		glNormal3f(gmodel_.normals_[i][0], gmodel_.normals_[i][1],
				gmodel_.normals_[i][2]);
		for (int j = 0; j < 3; ++j) {
			glVertex3f(gmodel_.vertices_[gmodel_.triangles_[i][j]][0],
					gmodel_.vertices_[gmodel_.triangles_[i][j]][1],
					gmodel_.vertices_[gmodel_.triangles_[i][j]][2]);
		}
	}
	glEnd();
glPopMatrix();


	 //draw axis;
glPushMatrix();

	glScalef(2.0f, 2.0f, 2.0f);

	glBegin(GL_LINES);
	for (int i = 0; i < 3; ++i) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, gmodel_.axismat[i]);
		for (int j = 0; j < 6; ++j) {
			glVertex3f(gmodel_.axis_[i * 6 + j][0], gmodel_.axis_[i * 6 + j][1],
					gmodel_.axis_[i * 6 + j][2]);
		}
	}
	glEnd();
	glPopMatrix();
	shuttleTime ++;
	shuttleTime=((int)shuttleTime )%1600;



	//draw cubes;
	setmaterial("cube");
	for (int i = 0; i < g_scene_.animate_.pnum_; ++i) {

		GLfloat color[4] = { 0.1f * i + 0.1f, 0.0f, 1.0f - 0.1f * i, 0.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color);
		glPushMatrix();
		//glTranslatef(g_scene_.animate_.points_[i][0], g_scene_.animate_.points_[i][1], g_scene_.animate_.points_[i][2]);
		//Interpolated point
		point p1(g_scene_.animate_.points_[i][0],
				g_scene_.animate_.points_[i][1],
				g_scene_.animate_.points_[i][2]), p2(
				g_scene_.animate_.points_[(i + 1) % 8][0],
				g_scene_.animate_.points_[(i + 1) % 8][1],
				g_scene_.animate_.points_[(i + 1) % 8][2]), p3(
				g_scene_.animate_.points_[(i + 2) % 8][0],
				g_scene_.animate_.points_[(i + 2) % 8][1],
				g_scene_.animate_.points_[(i + 2) % 8][2]), p4(
				g_scene_.animate_.points_[(i + 3) % 8][0],
				g_scene_.animate_.points_[(i + 3) % 8][1],
				g_scene_.animate_.points_[(i + 3) % 8][2]);
//glColor3f(0.0f,0.0f,0.0f);
		glPointSize(4);

		glColor3f(0.4f, 0.0f, 0.6f);
		double t = 0.00000;
		point u, v;
		for (t; t < 1;)

		{

			u = CatmullRoll(t, p1, p2, p3, p4);
			t += 0.005;
			v = CatmullRoll(t, p1, p2, p3, p4);

			glLineWidth(0.5);

			glBegin(GL_LINES);
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(u.x, u.y, u.z);
			glVertex3f(v.x, v.y, v.z);

			glEnd();


		}
		glTranslatef(g_scene_.animate_.points_[i][0],
				g_scene_.animate_.points_[i][1],
				g_scene_.animate_.points_[i][2]);

		glScalef(gmodel_.cubescale_[0], gmodel_.cubescale_[1],
				gmodel_.cubescale_[2]);
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
	}



	g_scene_.animate_.update();
	//draw the curves here

	//draw plane;
	glPushMatrix();
	setmaterial("plane");
	glTranslatef(gmodel_.planetrans_[0], gmodel_.planetrans_[1],
			gmodel_.planetrans_[2]);
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

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	TwDraw();
	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}

void createTable() {
	float distance = 0.0;
	float totalDistance = 0.0;

	table[0][0] = 0.00;
	table[0][2] = 0.00;
	table[0][1] = 0.00;
	int j = 1;
	for (int i = 0; i < g_scene_.animate_.pnum_; ++i) {

		point p1(g_scene_.animate_.points_[i][0],
						g_scene_.animate_.points_[i][1],
						g_scene_.animate_.points_[i][2]), p2(
						g_scene_.animate_.points_[(i + 1) % 8][0],
						g_scene_.animate_.points_[(i + 1) % 8][1],
						g_scene_.animate_.points_[(i + 1) % 8][2]), p3(
						g_scene_.animate_.points_[(i + 2) % 8][0],
						g_scene_.animate_.points_[(i + 2) % 8][1],
						g_scene_.animate_.points_[(i + 2) % 8][2]), p4(
						g_scene_.animate_.points_[(i + 3) % 8][0],
						g_scene_.animate_.points_[(i + 3) % 8][1],
						g_scene_.animate_.points_[(i + 3) % 8][2]);
		//glColor3f(0.0f,0.0f,0.0f);
				glPointSize(4);

				glColor3f(0.4f, 0.0f, 0.6f);
				double t = 0.00000;
				point u, v;
				for (t; t < 1;)

				{

					u = CatmullRoll(t, p1, p2, p3, p4);
					t += 0.005;
					v = CatmullRoll(t, p1, p2, p3, p4);

			distance += sqrt(
					(u.x - v.x) * (u.x - v.x) + (u.y - v.y) * (u.y - v.y)
							+ (u.z - v.z) * (u.z - v.z));
			table[j][0] = i;
			table[j][1] = t;
			table[j][2] = distance;
			table[j][3] = u.x;
						table[j][4] = u.y;
						table[j][5] = u.z;
			j++;
		}

	}

	totalDistance = table[1600][2];
	for (int i = 0; i <= 1600; i++) {
		table[i][2] = table[i][2] / totalDistance;

	}
}





void init() {
	gmodel_.init();
	for (int i = 0; i < g_scene_.animate_.pnum_; ++i) {
		gmodel_.planetrans_[1] = std::min(gmodel_.planetrans_[1],
				g_scene_.animate_.points_[i][1]);
	}
	gmodel_.planetrans_[1] -= 1;
	createTable() ;
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
	TwAddVarRW(operbar, "Rot X", TW_TYPE_FLOAT, &g_scene_.camera_.rot[0],
			"group='Camera Transfromation' min=-360 max=360 step=5");
	TwAddVarRW(operbar, "Rot Y", TW_TYPE_FLOAT, &g_scene_.camera_.rot[1],
			"group='Camera Transfromation' min=-360 max=360 step=5");
	TwAddVarRW(operbar, "Rot Z", TW_TYPE_FLOAT, &g_scene_.camera_.rot[2],
			"group='Camera Transfromation' min=-360 max=360 step=5");
	TwAddVarRW(operbar, "T1", TW_TYPE_FLOAT, &g_scene_.animate_.t1,
			"group='Camera Transfromation' min=0 max=1 step=0.05");
	TwAddVarRW(operbar, "T2", TW_TYPE_FLOAT, &g_scene_.animate_.t2,
			"group='Camera Transfromation' min=0 max=1 step=0.05");

	TwAddSeparator(operbar, "", "");
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
	glutTimerFunc(1600 / g_scene_.animate_.fps, Timer, value);
}
int main(int argc, char ** argv) {
	std::string filename;
	if (argc < 2) {
		//	throw std::logic_error("Input the moving object filaname;\n");
		filename = "C://Users//tarun//OneDrive - The University of Texas at Dallas//fall 2017//Animation//assignment 3//shuttle.obj";
	} else {
		filename = argv[1];
	}
	gmodel_.load_obj(filename);
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
