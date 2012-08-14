#ifndef WIDGET_H
#define WIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QtGui>
#include <QFile>
//#include <QCoreApplication>
#include <QTextStream>
#include "Qextserialport/src/qextserialport.h"
#include "Qextserialport/src/qextserialenumerator.h"
#include "MyLabel.h"
#include <math.h>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0); //the creator
    ~Widget();  //destructor
	QWidget* fenetre;

    

public slots:
    void EnvoyerCommandeAvancer();
	void EnvoyerCommandeTourner();
	void EnvoyerCommandeAller();
	void EnvoyerRobotParametres();
	void EnvoyerPidParametres();

	void init_port();
	void ActualiserPorts();
	void Connection();

	void mapUpdate();
	void mapClear();
	void ReceiveData();

	void MouseXY(int x, int y)
	{
		commandeAller[0]->setValue((320*x/label->width() -160)*10);
		commandeAller[1]->setValue(-(200*y/label->height() -100)*10);
		
	}// sending function

private :
    QSlider *slider;
	QComboBox *listeCOM;
	QPushButton *boutonConnection;
	QPushButton *boutonActualiser;
	QPixmap* Map;
	QPixmap MapEcran;
	MyLabel* label;

	QTextEdit *sortieProgramme;

	bool isConnection;
	QextSerialPort *port;
	QList<QextPortInfo> listePorts;
	// Commandes Robot
	 QDoubleSpinBox *commandeAvancer;
	 QDoubleSpinBox *commandeTourner;
	 QDoubleSpinBox *commandeAller[2];

	// Parametres Robot
	QDoubleSpinBox *acceleration;
    QDoubleSpinBox *deceleration;
	QDoubleSpinBox *roueDroite;
	QDoubleSpinBox *roueGauche;
    QSpinBox *roueCodeuse;
	QDoubleSpinBox *ecartRoue;

	QDoubleSpinBox* posX;
	QDoubleSpinBox* posY;
	QDoubleSpinBox* theta;

	// Parametres PID
	QDoubleSpinBox* coeffPID[4][3];

	

	QLayout* CreerPartieCommande();
	QLayout* CreerPartieConfiguration();
	QLayout* CreerPartieDeplacement();
	QLayout* CreerPartieGraphs();

   

	void resizeEvent(QResizeEvent* event)
	{
		//posX->setValue(label->height());//this->height());
		//posY->setValue(label->width());
		label->setPixmap(Map->scaled ( QSize((this->width()+24)/2, 2*(this->height()-6)/5)));

	}
};

#endif // WIDGET_H
