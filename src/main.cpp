#include <iostream>
#include <SFML/Graphics.hpp>
#include "Carte2D/Carte2D.h"
#include "2D/Objet.h"

using namespace std;

int main()
{
    Carte2D carte("ressources/terrain2012.png", 93,123,123.0/500, 123.0/500);
    carte.Launch();

    sf::Image imageCroix;
    if (!imageCroix.LoadFromFile("ressources/croix.tga"))
        return EXIT_FAILURE;

    sf::Sprite spriteCroix(imageCroix);
    spriteCroix.SetCenter(4.5,4.5);

    Objet point;
    point.x=0;
    point.y=0;
    carte.AjouterObjet(&point, &spriteCroix);

    char a;
    cout << "Entrer un caractere pour continuer" << endl;
    cin>>a;

    Objet point2;
    point.x=100;
    point.y=10;
    carte.AjouterObjet(&point2, &spriteCroix);

    cout << "Entrer un caractere pour continuer" << endl;
    cin>>a;

    carte.RetirerObjet(&point);

    cout << "Entrer un caractere pour continuer" << endl;
    cin>>a;

    return 0;
}