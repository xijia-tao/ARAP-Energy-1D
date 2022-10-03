
/*
 * Spring connecting two particles
 * 
 * Written by: Sean Brown
 * 
 */

#define _GLFW_X11
#define _GLFW_GLX
#define _GLFW_USE_OPENGL
#define _GLFW_HAS_GLXGETPROCADDRESS
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <iostream>

#include "Spring.h"

Spring::Spring() {

}

Spring::Spring(int i, int j, double k, double l, std::vector<Particle> particles) {
	this->i = i;
	this->j = j;
	this->k = k;
	this->l = l;

	Vector3d pi = particles[i].getPosition();
	Vector3d pj = particles[j].getPosition();
	this->dfdx = (pi - pj) / l;
}

void Spring::render(std::vector<Particle> particles) {
	particles[i].render();
	particles[j].render();

	Vector3d pi = particles[i].getPosition();
	Vector3d pj = particles[j].getPosition();

	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(pi[0], pi[1], pi[2]);
	glVertex3f(pj[0], pj[1], pj[2]);
	glEnd();
}

int Spring::getFirst() {
	return i;
}

int Spring::getSecond() {
	return j;
}

double Spring::getConstant() {
	return k;
}

double Spring::getLength() {
	return l;
}
Vector3d Spring::get_dfdx() {
	return dfdx;
}
void Spring::setFirst(int i) {
	this->i = i;
}

void Spring::setSecond(int j) {
	this->j = j;
}

void Spring::setConstant(double k) {
	this->k = k;
}

void Spring::setLength(double l) {
	this->l = l;
}
