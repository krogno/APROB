#ifndef OBJET_H
#define OBJET_H

/**
Un objet est un point présentant une orientation.
Note :
le repère utilisé ici est un repère trigonométrique classique avec normale z vers le haut : x de gauche à droite, y de bas en haut et theta de x vers y
SFML utilise un axe y inversé (y de haut en bas)
*/
class Objet
{
public:
    ///Position et rotation de l'objet (en radian)
    double x, y, theta;
};

#endif
