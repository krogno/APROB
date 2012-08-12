#include "Mobile.h"

#include <cmath>
#include <SFML/System.hpp>

void Mobile::Deplacer(double delta_avance, double delta_theta)
{
    static sf::Clock horloge;
    double theta_plus_delta_theta_2=theta+delta_theta/2;
    x+=delta_avance*cos(theta_plus_delta_theta_2);
    y+=delta_avance*sin(theta_plus_delta_theta_2);
    theta+=delta_theta;
    float tempsEcoule=horloge.GetElapsedTime();
    horloge.Reset();
    vitesse=delta_avance/tempsEcoule;
    omega=delta_theta/tempsEcoule;

}
