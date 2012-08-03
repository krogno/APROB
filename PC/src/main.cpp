#include <iostream>
#include <SFML/Graphics.hpp>
#include "2D/PointAffichable.h"

using namespace std;

int main()
{
    sf::RenderWindow fenetre(sf::VideoMode(800, 600, 32), "carte");

    sf::Image imageCroix;
    if (!imageCroix.LoadFromFile("ressources/croix.tga"))
        return EXIT_FAILURE;

    PointAffichable point(imageCroix, 4.5,4.5);

    point.x=300;
    point.y=300;

    fenetre.Clear(sf::Color(255,255,255));
    point.Afficher(fenetre);
    fenetre.Display();

    cout << "Entrer un caractere pour continuer" << endl;
    char a;
    cin>>a;

    return 0;
}
