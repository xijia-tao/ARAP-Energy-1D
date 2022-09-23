/*
 * main.cpp
 * 
 * Written by: Sean Brown
 * 
 * Particle:
 * x, y, z - Vector3ds of 3 (position
 * m - real number (mass)
 * 
 * Spring:
 * index i, j - integer (particles)
 * k - real number (spring constant)
 * l - real number (distance of rest state)
 * 
 * Force on particle p
 *     Fp = sum of forces connected to p
 * 
 * Force between particles
 *     Fp-pi = -spring.k(||p - pi|| - spring.l)(p - pi)/(||p - pi||)
 * 
 */

#define _GLFW_X11
#define _GLFW_GLX
#define _GLFW_USE_OPENGL
#define _GLFW_HAS_GLXGETPROCADDRESS
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <Eigen/IterativeLinearSolvers>

#include "Spring.h"
#include "Particle.h"
#include "arap.h"
#include <cmath>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;
using namespace glm;

using Eigen::Matrix3d;
using Eigen::Vector3d;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::EigenSolver;

GLFWwindow* window;
fstream         file;

// flags
bool debug = true;
bool gravity;
bool simulation;

// settings
float nearPlane    = 1.0f;
float farPlane     = 1000.0f;
float fov          = 60.0f;
float damp         = 0.99f;
float timeStep     = 0.0001f;
float eps          = 1e-4f; // cannot set too low

// glfw info
int width             = 1024;
int height            = 768;
string windowName;

Vector3d camera;
vector<Particle> particles;
vector<Spring> springs;

void getGravity() {
    char input[256];

    file >> input;
    if(!string(input).compare(string("false"))) {
        gravity = false;
    }
    else {
        gravity = true;
    }
}

void getParticles(int num) {
    float x, y, z, w;

    for(int i = 0; i < num; i++) {
        file >> x;
        file >> y;
        file >> z;
        file >> w;
        particles.push_back(Particle(Vector3d(x, y, z), w));
    }
}

void getSprings(int num) {
    int i, j;
    float k, l;

    file.get();

    for(int a = 0; a < num; a++) {
        file >> i;
        file >> j;
        file >> k;
        file >> l;
        springs.push_back(Spring(i, j, k, l));
    }
}

void readFile(string filename) {
    char title[256];
    char mode;
    int amount;
    int stillParticle;

    file = fstream(filename.c_str(), fstream::in);

    if(!file.is_open()) {
        cout << "File couldn't be open" << endl;
        exit(EXIT_FAILURE);
    }

    file.getline(title, 256);
    windowName = string(title);

    // get gravity setting
    file >> mode;
    if(mode == 'g') {
        getGravity();
    }
    else {
        cout << "No gravity setting" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "getting particles" << endl;
    // get particles
    file >> mode;
    if(mode == 'p') {
        file >> amount;
        file >> stillParticle;
        getParticles(amount);
        if(stillParticle >= 0) {
            particles[stillParticle].toggleMovement();
        }
    }
    else {
        cout << "improper particle section" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "getting springs" << endl;
    // get springs
    file >> mode;
    if(mode == 's') {
        file >> amount;
        getSprings(amount);
    }
    else {
        cout << "improper spring section" << endl;
        exit(EXIT_FAILURE);
    }

    file.close();
    cout << "finished loading" << endl;
}

void init() {
    simulation = false;

    camera = Vector3d(0.0f, 0.0f, 0.0f);

    glPointSize(10.0f);
}

void update() {
    Vector3d v1, v2, v3;
    Particle *p1, *p2;
    Vector3d forceOverLength;
    Vector3d force;
    Vector3d springVector3d;
    float springLength;
    float distanceFromRest;
    float hookesValue;
    Vector3d dampeningForce;

	float alpha, energy, E_prev, t, len, lenp = 1e7;
    vector<Vector3d> positionsp, positions;
    int cnt = 0, n = particles.size();
    VectorXd grad;
    MatrixXd hessian;

    Eigen::ConjugateGradient<Eigen::MatrixXd, Eigen::Lower|Eigen::Upper> cg;

    // init_pos(positionsp, particles);
    for (unsigned int i = 0; i < particles.size(); i++)
        positionsp.push_back(particles[i].getPosition());
    while(1) {
        get_global(springs, particles, hessian, grad, E_prev);
        if (grad.norm() < eps) break;
        // if (abs(lenp - P.norm()) < eps) cnt += 1;
        // if (cnt > 5) break;
        cg.compute(hessian);
        VectorXd p = -cg.solve(grad);
        alpha = 1;
        do {
            update_pos(positionsp, positions, p, alpha);
            alpha = alpha / 2;
            energy = get_energy(springs, particles);
        } while (energy > E_prev);
        positionsp = positions;
        E_prev = energy;
        // lenp = P.norm();
    }
    set_pos(positions, particles);
    if(debug) {
        // cout << "gravity = " << gravity << endl;
        // cout << "v1 = ";
        // p1->getVelocity().print();
        // cout << "v2 = ";
        // p2->getVelocity().print();
        // cout << "hooke value = " << hookesValue << endl;
        // cout << "dampening force = ";
        // dampeningForce.print();
        // cout << "springVector3d = ";
        // springVector3d.print();
        // cout << "Spring length = " << springLength << endl;
        // cout << "force = ";
        // force.print();
        cout << "p1 = ";
        cout << p1->getPosition() << endl;
        cout << "p2 = ";
        cout << p2->getPosition() << endl;
    }

    // for(unsigned int i = 0; i < particles.size(); i++) {
    //     if(!particles[i].isStationary()) {
    //         if(gravity) {
    //             particles[i].setForce(particles[i].getForce() + Vector3d(0.0f, -9.81f, 0.0f) * particles[i].getMass());
    //         }
    //     particles[i].setVelocity(particles[i].getVelocity() + (particles[i].getForce() / (particles[i].getMass() * timeStep)));
    //     particles[i].setVelocity(particles[i].getVelocity() * damp);
    //     particles[i].setPosition(particles[i].getPosition() + (particles[i].getVelocity() * timeStep));
    //     particles[i].setForce(Vector3d(0.0f, 0.0f, 0.0f));
    //     }
    // }
}

void render() {
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, height/width, nearPlane, farPlane);
    //gluLookAt(camera[0], camera[1], camera[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glTranslatef(camera[0], camera[1], camera[2]);

    for(unsigned int i = 0; i < springs.size(); i++) {
      springs[i].render(particles);
    }
    
    glPopMatrix();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void keyboardFunc(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS) {
        if(key == GLFW_KEY_ENTER) {
            update();
        }
        if(key == GLFW_KEY_SPACE) {
            simulation = !simulation;
        }
        if(key == GLFW_KEY_D) {
            debug = !debug;
        }
        if(key == GLFW_KEY_G) {
            gravity = !gravity;
        }
        if(key == GLFW_KEY_DOWN) {
            camera[2] = camera[2] - 10.0f;
        }
        if(key == GLFW_KEY_UP) {
            camera[2] = camera[2] + 10.0f;
        }
        if(key == GLFW_KEY_LEFT) {
            camera[0] = camera[0] - 10.0f;
        }
        if(key == GLFW_KEY_RIGHT) {
            camera[0] = camera[0] + 10.0f;
        }
        if(key == GLFW_KEY_P) {
            for(unsigned int i = 0; i < springs.size(); i++) {
                cout << particles[springs[i].getFirst()].getPosition() << endl;
                cout << particles[springs[i].getSecond()].getPosition() << endl;
            }
        }
    }
}

int main(int argc, char **argv)
{
    
    if(argc != 2) {
        cout << "Need a file." << endl;
        exit(EXIT_FAILURE);
    }

    readFile(string(argv[1]));

  if(!glfwInit()) {
        cout << "glfw failed to initialize" << endl;
    exit(EXIT_FAILURE);
  }
  
  window = glfwCreateWindow(width, height, windowName.c_str(), NULL, NULL);
  if(!window) {
        cout << "Window failed to be created" << endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
    
  glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyboardFunc);

  init();
    
  while(!glfwWindowShouldClose(window)) {
        if(simulation) {
            update();
        }

        render();
    }
    
  glfwDestroyWindow(window);
  glfwTerminate();
  
  return 0;
}

