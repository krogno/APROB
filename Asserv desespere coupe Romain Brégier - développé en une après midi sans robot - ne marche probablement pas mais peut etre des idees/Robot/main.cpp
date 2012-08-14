#include <iostream>
#include "Robot.h"
#include <sstream>
#include <thread>


using namespace std;



int main()
{
    /*rayon moyen des roues codeuses, ajustement (si le robot part a droite ou a gauche), distance entre les roues codeuses,
    port serie, baud rate, periode d asservissement en position (en ms)*/
    Robot bot(100,0,100,256,"/dev/ACM0", 115200, 200);

///*
    //Fait tourner les deux moteurs en avant  pendant 3s et les arrete
    cout<<"avant"<<endl;
    bot.EnvoyerConsigneVitesses(3,3);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    cout<<"stop"<<endl;
    bot.EnvoyerConsigneVitesses(0,0);
//*/

    //Si ca marche, regler le PID de l'arduino pour avoir une reponse en vitesse correcte
    //Puis virer le code ci dessus en vitesse et decommenter celui en dessous
/*
    bot.StartThread();//Lance la boucle d asservissement en position
    //1 - Regler d abord l ajustement (parametres du constructeur) pour que le robot aille droit (modifier la constante VITESSE_MAX dans Robot.h au passage)
    //2 - Regler le rayon moyen pour que le robot parcoure la bonne distance (tester avec une distance importante pour plus de precision)
    bot.GotoPosition(2000,0);
    getchar(); //C est juste pour que le programme ne se termine pas sinon ca coupe l asservissement et arrete le robot
*/
/*
    //Si ca marche, recommenter le code au dessus et decommenter celui la
    bot.StartThread();//Lance la boucle d asservissement en position
    bot.GotoOrientation(6*M_PI);//Dit au robot de s orienter de faire 3 tours sur lui meme
    getchar(); //C est juste pour que le programme ne se termine pas sinon ca coupe l asservissement et arrete le robot
    //3 - Regler la distance entre les roues pour que l angle qu il fasse soit bon, et regler le PID d'angle (constantes dans Robot.h)
*/

    //Si ca marche c est gagne!!
//Exemple de code
/*
        bot.StartThread();//Lance la boucle d asservissement en position
        bot.GotoPosition(2000,0);
        while(!bot.isBonnePosition()){} //Fait rien tant qu'il est pas arrive a la position
        bot.GotoPosition(2000,1000);
        while(!bot.isBonnePosition()){} //Fait rien tant qu'il est pas arrive a la position
        bot.GotoOrientation(M_PI);
        while(!bot.isBonneOrientation()){} //Fait rien tant qu'il est pas arrive a l'orientation voulue
*/

}














