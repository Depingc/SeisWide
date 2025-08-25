#pragma once

double *shepard_basis_1d(int nd, double xd[], double p, int ni, double xi[]);
//double *shepard_value_1d(int nd, double xd[], double yd[], double p, int ni, double xi[]);
void shepard_value_1d(int, double[], double[], double, int, double[], double[]);
float shepard_value_1dT();
