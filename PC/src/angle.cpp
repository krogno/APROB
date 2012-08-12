#include <cmath>
#include "angle.h"

double BornerA_MoinsPi_Pi(double angle)
{
    while(angle<-M_PI || angle>=M_PI)
    {
            (angle<0)? angle+=2*M_PI : angle-=2*M_PI;
    }
    return angle;
}
