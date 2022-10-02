#ifndef _ARAP_H_
#define _ARAP_H_

#include <iostream>
#include <vector>
#include "Spring.h"
#include <Eigen/Dense>

using Eigen::Vector3d;
using Eigen::Matrix3d;

Vector3d compute_F(Vector3d x1, Vector3d x2, float rest);
float compute_trace(Vector3d F);
float compute_energy(float t);
Vector3d sum_over_e(Matrix3d e, Matrix3d e_prime, Matrix3d i);
void compute_grad_hessian(Vector3d &, Matrix3d &H, float t, Vector3d F);

#endif