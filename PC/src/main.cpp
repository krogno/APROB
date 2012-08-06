#include <iostream>
#include <SFML/Graphics.hpp>
#include "Carte2D/Carte2D.h"
#include "2D/Mobile.h"

using namespace std;

int main()
{
    //Chargement des images et création des sprites pour l'affichage
    sf::Image imageCroix;
    if (!imageCroix.LoadFromFile("ressources/croix.tga"))
        return EXIT_FAILURE;
    sf::Sprite spriteCroix(imageCroix);
    spriteCroix.SetCenter(4.5,4.5);
    spriteCroix.SetScale(50.0/imageCroix.GetWidth(), 50.0/imageCroix.GetHeight());

    sf::Image imageBete;
    if (!imageBete.LoadFromFile("ressources/bete.png"))
        return EXIT_FAILURE;
    sf::Sprite spriteBete(imageBete);
    spriteBete.SetCenter(40,30);
    //Bete de 350mm  par 300mm
    spriteBete.SetScale(350.0/imageBete.GetWidth(), 300.0/imageBete.GetHeight());

    //Création de la carte
    Carte2D carte("ressources/terrain2012.png", 93,123,500.0/123, 500.0/123);
    carte.Launch();


    Objet point;
    point.x=2600;
    point.y=1500;
    carte.AjouterObjet(&point, &spriteCroix);

    char a;
    Mobile bete;
    carte.AjouterObjet(&bete, &spriteBete);

    cout << "Entrer un caractere pour continuer" << endl;
    cin>>a;
    //Avance d'un metre
    bete.Deplacer(1000, 0);

    cout << "Entrer un caractere pour continuer" << endl;
    cin>>a;
    //Tourne de -90°
    bete.Deplacer(0, -M_PI/2);

    cout << "Entrer un caractere pour continuer" << endl;
    cin>>a;
    //Avance d'un mètre
    bete.Deplacer(1000, 0);

    cout << "Entrer un caractere pour continuer" << endl;
    cin>>a;
    //Juste pour test : tourne de -45°, recule d'un metre et tourne de -45°
    bete.Deplacer(-1000, -M_PI/2);

    cout << "Entrer un caractere pour continuer" << endl;
    cin>>a;

    return 0;
}
