#ifndef MESSAGE_H
#define MESSAGE_H

#include<queue>
#include<string>
#include <SFML/System.hpp>
#include "serialib/serialib.h"
#include <iostream>

/**
Classe gerant l echange de messages avec une carte arduino, en envoyant des messages via port série de la forme "<message>"

**/
class Message : public sf::Thread
{
    public:
    Message(const char *port, unsigned int baud_rate);
    ~Message();

    void SendMessage(const std::string  message);
    //Renvoie le premier message recu dans la file de message
    std::string GetMessage();


    private:

    ///Adresse du port serie de la carte arduino
    const char *device;
    ///Baud rate du port
    unsigned int bauds;
    ///Instance gerant la communication serie.
    serialib serial;

    bool continuer;
    void Run();

    ///file de messages recus a traiter
    std::queue<std::string> file;

    sf::Mutex mutex_file, mutex_serial;
};

Message::Message(const char *port, unsigned int baud_rate)
{
    device=port;
    bauds=baud_rate;
    continuer=true;

    //Ouverture du port serie
    if(serial.Open(device, bauds)!=1)
    {
        //Renvoi un message si erreur
        std::cout<<"Message::Erreur d'ouverture du port serie : "<<device<<" a la frequence "<<bauds<<std::endl;
        exit(0);
    }
    Launch();
}

Message::~Message()
{
    continuer=false;
    Wait();
}

void Message::Run()
{
    while(continuer)
    {

        char message[100];
        int ret;
        std::string str="";

        //Lecture de la ligne arrivant
        //L ecriture et la lecture en communication sont independants donc on peut les utiliser en parallele dans deux threads differents
        ret=serial.ReadString(message, '\n', 100,0);

        //Si on a bien recupere une ligne
        if(ret>0)
        {
            //Conversion du retour en string
            std::string str(message);

            //La reception est terminee : ajout du message a la file de message
            {
                sf::Lock lock(mutex_file);
                file.push(str);
            }
        }
    }
}

std::string Message::GetMessage()
{
    sf::Lock lock(mutex_file);
    if(!file.empty())
    {
        //Lecture du message en début de file
        std::string message=file.front();
        //Suppression du message en question
        file.pop();
        //Renvoi du message
        return message;
    }
    else return "";
}

void Message::SendMessage(const std::string  message)
{
    //std::cout<<"send "<<message<<"\n";
    serial.WriteString(message.c_str());
}

#endif
