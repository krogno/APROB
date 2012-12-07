#include "TraitementWebcam.h"
#include <iostream>
#include <opencv2/opencv.hpp>

TraitementWebcam::TraitementWebcam(int numero, bool affichage)
{
    numeroWebcam=numero;
    affichageImage=affichage;
    continuer=true;
}

void TraitementWebcam::Run()
{
    cv::VideoCapture capture(numeroWebcam);
    if(!capture.isOpened())  // check if we succeeded
    {
        std::cout<<"Impossible d ouvrir en capture la webcam "<<numeroWebcam<<std::endl;
        return;
    }

    //Si demande d affichage, creation d une fenetre
if(affichageImage)
        cv::namedWindow( "cam", CV_WINDOW_AUTOSIZE );

    cv::Mat imgWebcam;

    while(continuer)
    {
        capture >> imgWebcam;
        std::cout<<"AFFICHAGE "<<affichageImage<<std::endl;
        //std::cout << imgWebcam<<std::endl;
        if(affichageImage)
            cv::imshow("cam", imgWebcam);

        cv::waitKey(30);
    }

}

void TraitementWebcam::Stop()
{
    continuer=false;
    Wait();
}

TraitementWebcam::~TraitementWebcam()
{
    Stop();
}
