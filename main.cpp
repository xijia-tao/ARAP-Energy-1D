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
        springs.push_back(Spring(i, j, k, l, particles));
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
    Vector3d v1, v2, v3; // for dampening
    Particle *p1, *p2;
    Vector3d forceOverLength;
    Vector3d force;
    Vector3d springVector3d;
    float springLength;
    float distanceFromRest;
    float hookesValue;
    Vector3d dampeningForce;

	float alpha, energy, t, len, lenp;
	Vector3d x1, x2, xp1, xp2, F, P, p;
    Matrix3d H;
    int cnt;

    Eigen::ConjugateGradient<Eigen::MatrixXd, Eigen::Lower|Eigen::Upper> cg;

    for(unsigned int i = 0; i < springs.size(); i++) {
        p1 = &particles[springs[i].getFirst()];
	    p2 = &particles[springs[i].getSecond()];

        len = springs[i].getLength();
		xp1 = p1->getPosition();
		xp2 = p2->getPosition();
        F = compute_F(xp1, xp2, len);
		t = compute_trace(F); 
		float E_prev = compute_energy(t);
        cnt = 0;
        lenp = 1e7;
		while (1) {
			compute_grad_hessian(P, H, t, F);
            // change the condition here to stop when P stops decreasing
			if (P.norm() < eps) break;	
            if (abs(lenp - P.norm()) < eps) cnt += 1;
            if (cnt > 5) break;
            cg.compute(H);
            Vector3d p = -cg.solve(P);

			alpha = 1;
			do {
                // this is wrong
				x1 = xp1 + alpha * p;
				x2 = xp2 - alpha * p;
				alpha = alpha / 2;

				F = compute_F(x1, x2, len); 
				t = compute_trace(F); 
				energy = compute_energy(t);
			} while (energy > E_prev);
			xp1 = x1;
			xp2 = x2;
			E_prev = energy;
            lenp = P.norm(); // previous PK1's norm
		}
        // to ensure a fixed point
        if(!p1->isStationary()) p1->setPosition(x1);
        if(!p2->isStationary()) p2->setPosition(x2);
        
        if(debug) {
            cout << "p1 = ";
            cout << p1->getPosition() << endl;
            cout << "p2 = ";
            cout << p2->getPosition() << endl;
        }
    }

    for(unsigned int i = 0; i < particles.size(); i++) {
        if(!particles[i].isStationary()) {
            if(gravity) {
                particles[i].setForce(particles[i].getForce() + Vector3d(0.0f, -9.81f, 0.0f) * particles[i].getMass());
            }
        particles[i].setVelocity(particles[i].getVelocity() + (particles[i].getForce() / (particles[i].getMass() * timeStep)));
        particles[i].setVelocity(particles[i].getVelocity() * damp);
        particles[i].setPosition(particles[i].getPosition() + (particles[i].getVelocity() * timeStep));
        particles[i].setForce(Vector3d(0.0f, 0.0f, 0.0f));
        }
  }
  // cout << "done" << endl;
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

