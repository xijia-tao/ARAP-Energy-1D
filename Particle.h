#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include <vector>
#include <Eigen/Dense>
using Eigen::Vector3d;

class Particle {
    private:
        bool stationary;
        Vector3d pos;
        Vector3d velocity;
        Vector3d color;
        Vector3d force;
        double mass;

    public:
        Particle(); // defaults: color = white, mass = .0f, pos = (0, 0, 0)
        Particle(Vector3d); // (pos)
        Particle(Vector3d, Vector3d); // (pos, color)
        Particle(Vector3d, double); // (pos, mass)
        Particle(Vector3d, Vector3d, double); // (pos, color, mass)

        void render();
        
        void toggleMovement();

        bool isStationary();
        Vector3d getPosition();
        Vector3d getVelocity();
        Vector3d getColor();
        Vector3d getForce();
        double getMass();

        void setStationary(bool);
        void setPosition(Vector3d);
        void setPosition(double, double, double);
        void setVelocity(Vector3d);
        void setVelocity(double, double, double);
        void setColor(Vector3d);
        void setColor(double, double, double);
        void setForce(Vector3d);
        void setForce(double, double, double);
        void setMass(double);
};

#endif
