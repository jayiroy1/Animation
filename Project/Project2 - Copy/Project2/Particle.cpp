#include "Particle.h"
#include "glut.h"
#include "V_Randomizer.h"
#include <math.h>

V_Randomizer randy1;
Particle::~Particle(){
}

Particle::Particle(double _posX, double _posY, double _posZ, double _velX, double _velY, double _velZ, float colR, float colG, float colB, float Particlesize){

	partType = 1;
	size = Particlesize;

	pos.x = _posX;
	pos.y = _posY;
	pos.z = _posZ;

	vel.x = _velX;
	vel.y = _velY;
	vel.z = _velZ;

	acc.x = 0.0;
	acc.y =-9.8;
	acc.z = 0.0;

	colour[0]=colR;
	colour[1]=colG;
	colour[2]=colB;

	alpha = 1;
	age = 0;
	partType =1;

}
void Particle::setColour(float r,float g, float b){
	colour[0] = r;
	colour[1] = g;
	colour[2] = b;
}
void Particle::setSize(float newSize){
	size = newSize;
}
void Particle::Update(double time){

	V_Randomizer randNumber;
	pos.x += (vel.x * time) + (0.5 * (acc.y * (time * time)));
	vel.x = vel.x + (acc.x * time);
	pos.y += (vel.y * time) + (0.5 * (acc.y * (time * time)));
	vel.y = vel.y + (acc.y * time);
	pos.z += (vel.z * time) + (0.5 * (acc.y * (time * time)));
	vel.z = vel.z + (acc.z * time);

	float Rdistance = (pos.x*pos.x + pos.z*pos.z);
	//Boundaries
	if (pos.y <=-50 && (Rdistance<90)){
		vel.y =-0.45*(vel.y);
		
	}
	if (pos.y <= -63 && (Rdistance)<480) {
		vel.y = -0.45*(vel.y);

	}
	if (pos.y <= -65 && pos.y>-63 && (Rdistance)<600 && (Rdistance)>475) {
		vel.x = -0.8*(vel.x);
		vel.z = -0.8*(vel.z);
		pos.x = 0;
		pos.z = 0;
		pos.y = -50;

	}
	float tan = pos.x / pos.z;
	float tanAngle = (atan(pos.x / pos.z)) * 180 / 3.1415;
	float rem = (int)(tanAngle) % 30;
	float multi = (int)(tanAngle / 30);

	//if (pos.y < -65)
	//{
	//	int p = 3;
	//	if (!(rem <= 2 && rem >= -2)) {

	//	//vel.y = randy1.rand(15.0, 20.0);
	//	vel.x = 3*sin(multi * 30 * 3.14 / 180);
	//	vel.z = 3* cos(multi * 30 * 3.14 / 180);
	//	pos.x =22*sin(multi*30 *3.14/180);
	//	pos.z = 22 * cos(multi * 30 * 3.14 / 180);
	//	
	//}
	//}
	if (pos.y <=-88 && Rdistance<950) {
		vel.y = -0.3*(vel.y);
	}
	if (pos.y <= -90) {
		vel.y = randy1.rand(5.0,15.0);
		vel.x = randy1.rand(-3.0,3.0); 
		vel.z=randy1.rand(-3.0,3.0);//-0.5*(vel.y);
		pos.x = 0;
		pos.z = 0;
		pos.y = -50;
	}



}

void Particle::setParticleType(int a){
	partType = a;
}
void Particle::Render(){

	V_Randomizer randNumber;
	glPushMatrix();

	glColor4f(colour[0],colour[1],colour[2],alpha);
	glTranslated(pos.x,pos.y,pos.z);
	glRotated(1.0,1.0,1.0,0.0);
	switch (partType){
		case 1:
			glutSolidSphere(size,15,15);
			break;
		case 2:
			glutSolidTeapot(size);
			break;
		case 3:
			glutSolidCube(size);
			break;
		case 4:
			glutWireSphere(size,15,15);
			break;
		case 5:
			glutWireCube(size);
			break;
		case 6:
			glutSolidCone(size,size,15,15);
			break;
		case 7:
			glutSolidTorus(size/2,size,15,15);
			break;
		case 8:
			glPointSize(size*2);
			glBegin(GL_POINTS);
			glVertex3f(0,0,0);
			glEnd();
			break;
	}
	glPopMatrix();
}