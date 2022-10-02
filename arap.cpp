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