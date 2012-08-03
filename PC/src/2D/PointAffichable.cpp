#include "PointAffichable.h"

PointAffichable::PointAffichable(sf::Image& image, float xCentre, float yCentre)
{
    sprite.SetImage(image);
    sprite.SetCenter(xCentre, yCentre);
}

void PointAffichable::Afficher(sf::RenderWindow &fenetre)
{
    sprite.SetX(x);
    sprite.SetY(y);
    fenetre.Draw(sprite);
}
