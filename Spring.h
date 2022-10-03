#ifndef _SPRING_H_
#define _SPRING_H_

/*
 * Spring connecting two particles
 * 
 * Written by: Sean Brown
 * 
 */


// #include "Vector.h"
#include "Particle.h"

class Spring {
private:
	int i, j;
	double k;
	double l;
	Vector3d dfdx;

public:
	Spring();
	Spring(int, int, double, double, std::vector<Particle>); // i, j, k, length, particle list

	void render(std::vector<Particle>);

	int getFirst();
	int getSecond();
	double getConstant();
	double getLength();
	Vector3d get_dfdx();

	void setFirst(int);
	void setSecond(int);
	void setConstant(double);
	void setLength(double);
};

#endif
