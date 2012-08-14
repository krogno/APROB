#include <cmath>

//Renvoie une valeur congrue a theta entre -°I et PI
double DoubleToAngle(double theta)
{
    bool invalide=true;
    while(invalide)
    {
        if(theta<-M_PI)
        {
            theta+=2*M_PI;
        }
        else if (theta>=M_PI)
        {
            theta-=2*M_PI;
        }
        else invalide=false;
    }
    return theta;
}


