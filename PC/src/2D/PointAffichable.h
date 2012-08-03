#ifndef POINT_AFFICHABLE_H
#define POINT_AFFICHABLE_H

#include "Point.h"
#include <SFML/Graphics.hpp>

class PointAffichable : public Point
{
public :
    /**xCentre et yCentre position du point de l'image correspondant au centre de l'objet
    en coordonnees relatives au point en haut à gauche de l'image
    */
    PointAffichable(sf::Image& image, float xCentre, float yCentre);
    void Afficher(sf::RenderWindow& fenetre);

protected :
    sf::Sprite sprite;
};

#endif
