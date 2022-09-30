#include "arap.h"

float mu = 1.0f;
float lambda = 1.0f;
bool vol_preserv = false;
bool print = false;

Vector3d compute_F(Vector3d x1, Vector3d x2, float rest) {
    return (x1 - x2) / rest;
}

float compute_trace(Vector3d F) {
    return F.norm();
}

float compute_energy(float t) {
    float Ic = t * t;
    float psi = Ic - 2 * t + 1;
    float energy = mu * psi;
    if (vol_preserv) {
        float J = t;
        energy += lambda * (J - 1) * (J - 1);
    }
    return energy;
}

Vector3d sum_over_e(Matrix3d e, Matrix3d e_prime, int i) {
    int k = 0;
    Vector3d result;
    while (k < i) {
        float e_prime_k_squared = e_prime.col(i).dot(e_prime.col(i));
        result += ((e.col(i).dot(e_prime.col(k)) / e_prime_k_squared) * e_prime.col(k));
        std::cout << e_prime_k_squared << ' ';
        std::cout << e_prime.col(k) << ' ';
        k++;
    }
    return result;
}
// H obtained this way is not guaranteed to have an inverse
// solution: use CG isntead of direct calc
void compute_grad_hessian(Vector3d &P, Matrix3d &H, float t, Vector3d F) {
    P = mu * F * (1 - 1 / t);
    // H = mu / (t * t * t) * outerProduct(F, F) + mu * (1 - t) * Matrix3d(1.0f);
    Matrix3d U;
    Vector3d R = F.normalized();

    U.col(0) = R;
    Vector3d u;
    if (R[0] == 0.0 && R[1] == 0.0) u = Vector3d(R[2], 0, -R[0]);
    else if (R[0] == 0.0 && R[2] == 0.0) u = Vector3d(R[1], -R[0], 0);
    else if (R[0] == 0.0f && R[1] == 0.0f && R[2] == 0.0f) {
        std::cout << "Warning: 0 F" << std::endl;
        return;
    }
    else u = Vector3d(R[2], 0, -R[0]);
    U.col(1) = u - R.dot(u) * R; // R is normalized
    U.col(2) = U.col(0).cross(U.col(1));
    
    float l12 = std::max(1 - 1 / t, 0.0f); // the second and third eigenvalue
    H = U.col(0) * U.col(0).transpose() + l12 * U.col(1) * U.col(1).transpose() + l12 * U.col(2) * U.col(2).transpose();

    if (print) {
        std::cout << "F: " << F << std::endl;
        std::cin.get();
        std::cout << "R: " << R << std::endl;
        std::cin.get();
        std::cout << "U: " << U << std::endl;
        std::cin.get();
    }
}

// get the global hessian and the total energy
void get_global(vector<Spring> springs, vector<Particle> particles, MatrixXd &hessian, VectorXd &grad, float &energy) {
    int i1, i2, n = particles.size();
    float t, len;
    Vector3d P, F, xp1, xp2;
    Matrix3d H;
    Particle *p1, *p2;
    energy = 0;
    hessian = MatrixXd::Zero(3 * n, 3 * n);
    grad = VectorXd::Zero(3 * n); 
    for (unsigned int i = 0; i < springs.size(); i++) {
        i1 = springs[i].getFirst();
        i2 = springs[i].getSecond();
        p1 = &particles[i1];
	    p2 = &particles[i2];

        len = springs[i].getLength();
		xp1 = p1->getPosition();
		xp2 = p2->getPosition();
        F = compute_F(xp1, xp2, len);
		t = compute_trace(F); 
        compute_grad_hessian(P, H, t, F);
        for (int j = 0; j < 3; j++) {
            grad[3 * i1 + j] -= P[j]; // not sure if this is the correct way of calc a global gradient
            grad[3 * i2 + j] += P[j]; // but the only meaningful way that I can think of
        }
        hessian.block(3 * i1, 3 * i2, 3, 3) = H; // a 3nx3n matrix
        hessian.block(3 * i2, 3 * i1, 3, 3) = H; 
        energy += compute_energy(t);
    }
}

// get the total energy
// float get_energy(vector<Spring> springs, vector<Vector3d> positions) {
//     float energy = 0, len, t;
//     int i1, i2;
//     Vector3d F;
//     for (unsigned int i = 0; i < springs.size(); i++) {
//         i1 = springs[i].getFirst();
//         i2 = springs[i].getSecond();

//         len = springs[i].getLength();
//         F = compute_F(positions[i1], positions[i2], len);
// 		t = compute_trace(F); 
//         energy += compute_energy(t);
//     }
//     return energy;
// }

// void init_pos(vector<Vector3d> positions, vector<Particle> &particles) {
//     for (unsigned int i = 0; i < particles.size(); i++)
//         positions.push_back(particles[i].getPosition());
// }

void update_pos(vector<Vector3d> positionsp, vector<Vector3d> &positions, VectorXd p, float alpha) {
    for (unsigned int i = 0; i < positions.size(); i++) 
        positions[i] = positionsp[i] + alpha * p.segment(3 * i, 3);
}

void set_pos(vector<Vector3d> &positions, vector<Particle> particles) {
    for (unsigned int i = 0; i < particles.size(); i++)
        particles[i].setPosition(positions[i]);
}