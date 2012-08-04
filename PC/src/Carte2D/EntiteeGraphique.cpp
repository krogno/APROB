#include "EntiteeGraphique.h"

EntiteeGraphique::EntiteeGraphique(Objet* obj, sf::Sprite& sprit):
sprite(sprit)
{
    objet = obj;
}

void EntiteeGraphique::Afficher(sf::RenderWindow& fenetre)
{
    sprite.SetPosition(objet->x, objet->y);
    sprite.SetRotation(objet->theta);
    fenetre.Draw(sprite);
}


