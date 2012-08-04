#ifndef OBJET_H
#define OBJET_H

#include "Point.h"

/**
Un objet est un point présentant une orientation.
*/
class Objet : public Point
{
    public:
    ///Position et rotation de l'objet (en radian)
    double x, y, theta;
};

#endif
