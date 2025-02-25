#include "util.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

double randd(double min, double max)
{
    return min + (double)rand() / (RAND_MAX / (max - min));
}

double randd_dec(void)
{
    return randd(0.0, 1.0);
}

double relu(double x)
{
    return x > 0 ? x : 0;
}

double relu_deriv(double x)
{
    return x > 0 ? 1 : 0;
}

double sigmoid(double x)
{
    return 0.5 * (x / (1 + fabs(x)) + 1);
}

double sigmoid_deriv(double x)
{
    return x * (1 - x);
}
