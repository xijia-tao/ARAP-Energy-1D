#define _GLFW_X11
#define _GLFW_GLX
#define _GLFW_USE_OPENGL
#define _GLFW_HAS_GLXGETPROCADDRESS
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "Particle.h"


Particle::Particle() {// defaults: color = white, mass = 1.0f, pos = (0, 0, 0)
  	this->stationary = false;
	this->pos = Vector3d(0.0f, 0.0f, 0.0f);
	this->velocity = Vector3d(0.0f, 0.0f, 0.0f);
	this->color = Vector3d(1.0f, 1.0f, 1.0f);
	this->force = Vector3d(0.0f, 0.0f, 0.0f);
	this->mass = 1.0f;
}
  
Particle::Particle(Vector3d p) {
  this->stationary = false;
	this->pos = p;
	this->velocity = Vector3d(0.0f, 0.0f, 0.0f);
	this->color = Vector3d(1.0f, 1.0f, 1.0f);
	this->force = Vector3d(0.0f, 0.0f, 0.0f);
	this->mass = 1.0f;
}

Particle::Particle(Vector3d p, Vector3d c){
  this->stationary = false;
	this->pos = p;
	this->velocity = Vector3d(0.0f, 0.0f, 0.0f);
	this->force = Vector3d(0.0f, 0.0f, 0.0f);
	this->mass = 1.0f;
	this->color = c;
}
  
Particle::Particle(Vector3d p, double m) {
  this->stationary = false;
	this->pos = p;
	this->velocity = Vector3d(0.0f, 0.0f, 0.0f);
	this->color = Vector3d(1.0f, 1.0f, 1.0f);
	this->force = Vector3d(0.0f, 0.0f, 0.0f);
	this->mass = m;
}

Particle::Particle(Vector3d p, Vector3d c, double m) {
  this->stationary = false;
	this->pos = p;
	this->velocity = Vector3d(0.0f, 0.0f, 0.0f);
	this->force = Vector3d(0.0f, 0.0f, 0.0f);
	this->mass = m;
	this->color = c;
}

void Particle::render() {
	glBegin(GL_POINTS);
	glColor3f(color[0], color[1], color[2]);
	glVertex3f(pos[0], pos[1], pos[2]);
	glEnd();
}

void Particle::toggleMovement() {
  this->stationary = !stationary;
}

bool Particle::isStationary() {
  return this->stationary;
}

Vector3d Particle::getPosition() {
	return this->pos;
}

Vector3d Particle::getVelocity() {
	return this->velocity;
}

Vector3d Particle::getColor() {
	return this->color;
}

Vector3d Particle::getForce() {
	return this->force;
}

double Particle::getMass() {
	return this->mass;
}

void Particle::setStationary(bool s) {
  this->stationary = s;
}

void Particle::setPosition(Vector3d p) {
	this->pos = p;
}

void Particle::setPosition(double x, double y, double z) {
	this->pos = Vector3d(x, y, z);
}

void Particle::setVelocity(Vector3d v) {
	this->velocity = v;
}

void Particle::setVelocity(double x, double y, double z) {
	this->velocity = Vector3d(x, y, z);
}

void Particle::setColor(Vector3d c) {
	this->color = c;
}

void Particle::setColor(double r, double g, double b) {
	this->color = Vector3d(r, g, b);
}

void Particle::setForce(Vector3d f) {
	this->force = f;
}

void Particle::setForce(double x, double y, double z) {
	this->force = Vector3d(x, y, z);
}

void Particle::setMass(double m) {
	this->mass = m;
}
