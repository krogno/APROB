#include "Mobile.h"

#include <cmath>

void Mobile::Deplacer(double delta_avance, double delta_theta)
{
    double theta_plus_delta_theta_2=theta+delta_theta/2;
    x+=delta_avance*cos(theta_plus_delta_theta_2);
    y+=delta_avance*sin(theta_plus_delta_theta_2);
    theta+=delta_theta;
}
