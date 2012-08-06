#ifndef OBJET_H
#define OBJET_H

/**
Un objet est un point présentant une orientation.
Note :
Afin d'être consistant avec SFML, le meme type de repère est utilisé ici.
(x de gauche à droite, y de haut en bas et theta de x vers -y (correspond au sens trigonométrique classique pour une normale qui sort de l'écran)
*/
class Objet
{
public:
    ///Position et rotation de l'objet (en radian)
    double x, y, theta;
};

#endif
