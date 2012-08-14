#include "widget.h"


QLayout* Widget::CreerPartieDeplacement()
{
	QVBoxLayout* PartieDeplacement = new QVBoxLayout;
	Map = new QPixmap(320, 200);
	label = new MyLabel;
	label->setPixmap(*Map);

	PartieDeplacement->addWidget(label);


	QGridLayout *layoutXY = new QGridLayout;
	//layoutXY->setSpacing(20);

	QFormLayout *formLayout1 = new QFormLayout;
	posX = new QDoubleSpinBox;
	posX->setRange(-1000,1000);
	posX->setReadOnly(true);
    formLayout1->addRow("&X", posX);

	QFormLayout *formLayout2 = new QFormLayout;
	posY = new QDoubleSpinBox;
	posY->setRange(-1000,1000);
	posY->setReadOnly(true);
    formLayout2->addRow("&Y", posY);

	QFormLayout *formLayout3 = new QFormLayout;
	theta = new QDoubleSpinBox;
	theta->setRange(-1000,1000);
	theta->setReadOnly(true);
    formLayout3->addRow("&T", theta);

	QPushButton* EffaceMap = new QPushButton("Effacer map");

	layoutXY->addLayout(formLayout1, 0, 0);
	layoutXY->addLayout(formLayout2, 0, 1);
	layoutXY->addLayout(formLayout3, 0, 2);
	layoutXY->addWidget(EffaceMap, 1, 1);

	PartieDeplacement->addLayout(layoutXY);

	QObject::connect(label, SIGNAL(clicked(int,int)), this, SLOT(MouseXY(int,int)));
	QObject::connect(EffaceMap, SIGNAL(clicked()), this, SLOT(mapClear()));

	return PartieDeplacement;
}
QLayout* Widget::CreerPartieCommande()
{
	QVBoxLayout* PartieConnection = new QVBoxLayout;

	QGroupBox *groupBoxConnection = new QGroupBox;
	//groupBox->setFrameShape(QFrame::StyledPanel);
	groupBoxConnection->setTitle("Connection");
	//groupBoxConnection->setGeometry(30, 20, 120, 60);
	listeCOM = new QComboBox;
	boutonConnection = new QPushButton("Connecter");
	boutonActualiser = new QPushButton("Actualiser");

	QObject::connect(boutonConnection, SIGNAL(pressed()), this, SLOT(Connection()));
	QObject::connect(boutonActualiser, SIGNAL(pressed()), this, SLOT(ActualiserPorts()));

	QHBoxLayout* PartieConnection2 = new QHBoxLayout;
	PartieConnection2->addWidget(boutonConnection);
	PartieConnection2->addWidget(boutonActualiser);
	PartieConnection->addWidget(listeCOM);
	PartieConnection->addLayout(PartieConnection2);

	groupBoxConnection->setLayout(PartieConnection);



	QLayout* PartieCommande = new QVBoxLayout;
	QGroupBox *groupBox = new QGroupBox;
	//groupBox->setFrameShape(QFrame::StyledPanel);
	groupBox->setTitle("Commandes");
	
    QPushButton *boutonAvancer = new QPushButton("Avancer (mm)");
	QPushButton *boutonTourner = new QPushButton("Tourner (°)");
	QPushButton *boutonAller =   new QPushButton("Aller (X,Y)");
	QObject::connect(boutonAvancer, SIGNAL(pressed()), this, SLOT(EnvoyerCommandeAvancer()));
	QObject::connect(boutonTourner, SIGNAL(pressed()), this, SLOT(EnvoyerCommandeTourner()));
	QObject::connect(boutonAller, SIGNAL(pressed()), this, SLOT(EnvoyerCommandeAller()));

	commandeAvancer = new QDoubleSpinBox;
	commandeTourner = new QDoubleSpinBox;
	commandeAller[0] = new QDoubleSpinBox;
	commandeAller[1] = new QDoubleSpinBox;

	commandeAvancer->setRange(-5000,5000);
	commandeTourner->setRange(-180,180);
	commandeAller[0]->setRange(-5000,5000);
	commandeAller[1]->setRange(-5000,5000);

	QGridLayout *layoutOrdres = new QGridLayout;
	layoutOrdres->addWidget(boutonAvancer,0,0,0,1);
	layoutOrdres->addWidget(boutonTourner,1,0,1,1);
	layoutOrdres->addWidget(boutonAller  ,2,0,2,1);
	layoutOrdres->addWidget(commandeAvancer,0,2,0,3);
	layoutOrdres->addWidget(commandeTourner,1,2,1,3);
	layoutOrdres->addWidget(commandeAller[0],2,2);
	layoutOrdres->addWidget(commandeAller[1],2,3);

	layoutOrdres->setVerticalSpacing(30);
	//layoutOrdres->setHorizontalSpacing(0);


    groupBox->setLayout(layoutOrdres);
	PartieCommande->addWidget(groupBoxConnection);
	PartieCommande->addWidget(groupBox);
	return PartieCommande;
}
QLayout* Widget::CreerPartieConfiguration()
{
	QLayout* PartieConfiguration = new QVBoxLayout;
	QTabWidget *Onglets = new QTabWidget;
	
    QWidget *page1 = new QWidget;
    QWidget *page2 = new QWidget;


    // Page 1
	acceleration = new QDoubleSpinBox;
    deceleration = new QDoubleSpinBox;
	roueDroite = new QDoubleSpinBox;
	roueGauche = new QDoubleSpinBox;
	ecartRoue = new QDoubleSpinBox;
    roueCodeuse = new QSpinBox;

	roueCodeuse->setRange(0,10000);
	ecartRoue->setRange(0,300);
    QFormLayout *formLayout1 = new QFormLayout;
    formLayout1->addRow("&Accéleration", acceleration);
    formLayout1->addRow("&Déceleration", deceleration);
    formLayout1->addRow("Roue &droite (mm)", roueDroite);
	formLayout1->addRow("Roue &gauche (mm)", roueGauche);
	formLayout1->addRow("&Crans Roue codeuse", roueCodeuse);
	formLayout1->addRow("&Ecart roues (mm)", ecartRoue);
	
    QVBoxLayout *layoutOnglet1 = new QVBoxLayout;
    layoutOnglet1->addLayout(formLayout1); // Ajout du layout de formulaire

    QPushButton *boutonValider = new QPushButton("Valider");
    QWidget::connect(boutonValider, SIGNAL(clicked()), this, SLOT(EnvoyerRobotParametres()));
    layoutOnglet1->addWidget(boutonValider); // Ajout du bouton
    page1->setLayout(layoutOnglet1);


    // Page 2

	QGridLayout *layoutPID = new QGridLayout;
	
	QWidget *label1 = new QLineEdit("P");
	QWidget *label2 = new QLineEdit("I");
	QWidget *label3 = new QLineEdit("D");
	
	layoutPID->addWidget(label1,0,1);
	layoutPID->addWidget(label2,0,2);
	layoutPID->addWidget(label3,0,3);


	QLabel *labeld = new QLabel("PID droite");
	QLabel *labelg = new QLabel("PID gauche");
	QLabel *labelA = new QLabel("PID angle");
	QLabel *labelD = new QLabel("PID Distance");
	layoutPID->addWidget(labeld,1,0);
	layoutPID->addWidget(labelg,2,0);
	layoutPID->addWidget(labelA,3,0);
	layoutPID->addWidget(labelD,4,0);

	for(int i=0; i<4; i++)
	{
		for(int j=0; j<3; j++)
		{
			coeffPID[i][j] = new QDoubleSpinBox();
			coeffPID[i][j]->setDecimals(3);
			layoutPID->addWidget(coeffPID[i][j],1+i,1+j);
		}
	}

	
    QVBoxLayout *layoutOnglet2 = new QVBoxLayout;
    layoutOnglet2->addLayout(layoutPID); // Ajout du layout de formulaire

    QPushButton *boutonValider2 = new QPushButton("Valider");
    QWidget::connect(boutonValider2, SIGNAL(clicked()), this, SLOT(EnvoyerPidParametres()));
    layoutOnglet2->addWidget(boutonValider2); // Ajout du bouton
    page2->setLayout(layoutOnglet2);


    Onglets->addTab(page1, "Réglages Robot");
    Onglets->addTab(page2, "Réglages PID");

	PartieConfiguration->addWidget(Onglets);
	return PartieConfiguration;
}
QLayout* Widget::CreerPartieGraphs()
{
	QLayout* PartieCommande = new QVBoxLayout;
	QGroupBox *groupBox = new QGroupBox;
	groupBox->setTitle("Commandes");
	groupBox->setGeometry(30, 20, 120, 80);
	
	QProgressBar *progress = new QProgressBar;
    progress->setValue(50);
    slider = new QSlider(Qt::Horizontal);
	slider->setRange(0,100);
	slider->setValue(progress->value());
    QPushButton *bouton3 = new QPushButton("Valider");

	QObject::connect(slider, SIGNAL(valueChanged(int)), progress, SLOT(setValue(int)));

	
	sortieProgramme = new QTextEdit();

    QVBoxLayout *vbox2 = new QVBoxLayout;
    vbox2->addWidget(progress);
    vbox2->addWidget(slider);
    vbox2->addWidget(bouton3);
	vbox2->addWidget(sortieProgramme);

    groupBox->setLayout(vbox2);
	PartieCommande->addWidget(groupBox);
	return PartieCommande;
}



Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
	QGridLayout * layoutPrincipal = new QGridLayout;

	QLayout *PartieConfiguration = CreerPartieConfiguration();
	QLayout *PartieCommande = CreerPartieCommande();
	QLayout *PartieDeplacement = CreerPartieDeplacement();
	QLayout *PartieGraphs = CreerPartieGraphs();

	layoutPrincipal->addLayout(PartieConfiguration, 0, 0);
    layoutPrincipal->addLayout(PartieGraphs, 0, 1);
    layoutPrincipal->addLayout(PartieCommande, 1, 0);
	layoutPrincipal->addLayout(PartieDeplacement, 1, 1);
	
	this->setGeometry(100, 100, 1, 1);
	this->setLayout(layoutPrincipal);

	isConnection = false;
	port = new QextSerialPort;
	ActualiserPorts();
	init_port(); //initiation of the port

	QTimer* timer = new QTimer(this);
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(ReceiveData()));
	timer->start(50);
	
	QFile file("data.txt");
	file.open(QIODevice::ReadOnly | QIODevice::Text);
 
	QTextStream in(&file);

	
	QString line = in.readLine();
	int n=0;
	QString buff;

	int i=0;
	while(n<12)
	{
		buff.clear();
		while(line.at(i) != ',')
		{
			buff.push_back(line.at(i));
			i++;
		}
		i++;
			
		coeffPID[n%4][n-3*(n%4)]->setValue(buff.toDouble());
		//QMessageBox::warning(this, "port info", buff);
		n++;
	}
	
	buff.clear();
	while(line.at(i) != ',')
	{
		buff.push_back(line.at(i));
		i++;
	}
	i++;
	acceleration->setValue(buff.toDouble());

	buff.clear();
	while(line.at(i) != ',')
	{
		buff.push_back(line.at(i));
		i++;
	}
	i++;
	deceleration->setValue(buff.toDouble());

	buff.clear();
	while(line.at(i) != ',')
	{
		buff.push_back(line.at(i));
		i++;
	}
	i++;
	roueDroite->setValue(buff.toDouble());

	buff.clear();
	while(line.at(i) != ',')
	{
		buff.push_back(line.at(i));
		i++;
	}
	i++;
	roueGauche->setValue(buff.toDouble());

	buff.clear();
	while(line.at(i) != ',')
	{
		buff.push_back(line.at(i));
		i++;
	}
	i++;
	roueCodeuse->setValue(buff.toInt());

	buff.clear();
	while(line.at(i) != ',')
	{
		buff.push_back(line.at(i));
		i++;
	}
	i++;
	ecartRoue->setValue(buff.toDouble());
	file.close();
	mapClear();
}
Widget::~Widget()
{
	if(isConnection)
	{
		char portByte = 0x7F;
		port->write(&portByte, 1);
		port->close(); //we close the port at the end of the program
	}
	QFile file("data.txt");
	file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<3; j++)
		{
			out.setRealNumberPrecision(4);
			out<<coeffPID[i][j]->value()<<",";
		}
	}
	out<<acceleration->value()<<",";
	out<<deceleration->value()<<",";
	out<<roueDroite->value()<<",";
	out<<roueGauche->value()<<",";
	out<<roueCodeuse->value()<<",";
	out<<ecartRoue->value()<<",";

	file.close();

}


void Widget::ReceiveData()
{
	//QMessageBox::warning(this, "port info", "ReadPacket");
	if(!isConnection)
		return;

	if(port->bytesAvailable() >8)// 4 data + 1 controle
	{
		int ct = 9;//port->bytesAvailable();
		QByteArray buf2 = port->read(ct);//readAll();
		QString abcd(" ");
		char data[256];

		//abcd.sprintf("recu :%d ",ct);
		//	sortieProgramme->insertPlainText(abcd);
	
		for(int i=0; i<buf2.size(); i++)//-1 car \r et \n
		{
			data[i] = buf2.at(i);
		//	abcd.sprintf("%d,",data[i]);
		//	sortieProgramme->insertPlainText(abcd);

		}
		abcd.sprintf("%d\n", data[0]);
		sortieProgramme->insertPlainText(abcd);
		

		if(int(data[0]) == 0x10)
		{
			int n=0;
			while(4*n<buf2.size()-2)//-2 car \n et octet de commande
			{
				float* aaa = (float*)&data[4*n+1];
				abcd.sprintf("recu%d : %f\n", n,  *aaa);
				//abcd.sprintf("recu%d : 0x%x,0x%x,0x%x,0x%x\n", n,  data[4*n+1],data[4*n+2],data[4*n+3],data[4*n+4]);
				sortieProgramme->insertPlainText(abcd);
				n++;
			}
			posX->setValue((double(*(float*)&data[1])));
			posY->setValue((double(*(float*)&data[5])));
			mapUpdate();
		}
		else if(int(data[0]) == 0x11)
		{
			int n=0;
			while(4*n<buf2.size()-2)//-2 car \n et octet de commande
			{
				float* aaa = (float*)&data[4*n+1];
				abcd.sprintf("recu%d : %f\n", n,  *aaa);
				//abcd.sprintf("recu%d : 0x%x,0x%x,0x%x,0x%x\n", n,  data[4*n+1],data[4*n+2],data[4*n+3],data[4*n+4]);
				sortieProgramme->insertPlainText(abcd);
				n++;
			}
		}
		else if(int(data[0]) == 0x09)
		{
			for(int i=1; i<buf2.size(); i++)
			{
				abcd.sprintf("%c",data[i]);
				sortieProgramme->insertPlainText(abcd);
			}
			abcd.sprintf("\n");
			sortieProgramme->insertPlainText(abcd);
		}
		else
		{
			for(int i=1; i<buf2.size(); i++)
			{
				abcd.sprintf("%d,",data[i]);
				sortieProgramme->insertPlainText(abcd);
			}
			abcd.sprintf("\n");
			sortieProgramme->insertPlainText(abcd);
		}
		

		

		QTextCursor c = sortieProgramme->textCursor();
		c.movePosition(QTextCursor::End);
		sortieProgramme->setTextCursor(c);
	}

	//theta->setValue(slider->value());
}
void Widget::mapClear()//QPaintEvent *event)
{
	QPen pen(Qt::black, 1, Qt::SolidLine);

	QPainter painter(this->Map);
	Map->fill(Qt::blue);
	painter.setPen(pen);
	painter.drawLine(0, 100, 320, 100);
	painter.drawLine(160, 0, 160, 200);
	//mapUpdate();
}
void Widget::mapUpdate()
{
	static int RobotX = 0;
	static int RobotY = 0;

	QPen pen(Qt::red, 1, Qt::SolidLine);
	pen.setCapStyle(Qt::RoundCap);

	QPainter painter(this->Map);
	painter.setPen(pen);
	painter.drawLine(RobotX+160, RobotY+100,
					int(posX->value()/10)+160,
					-int(posY->value()/10)+100);
	label->setPixmap(Map->scaled ( QSize((this->width()+24)/2, 2*(this->height()-6)/5)));
	RobotX = int(posX->value()/10);
	RobotY = -int(posY->value()/10);
}
void Widget::init_port()
{
    //we set the port properties
    port->setBaudRate(BAUD9600);//modify the port settings on your own
    port->setFlowControl(FLOW_OFF);//FLOW_HARDWARE);//FLOW_OFF);//FLOW_XONXOFF);//FLOW_OFF);
    port->setParity(PAR_NONE);
    port->setDataBits(DATA_8);
    port->setStopBits(STOP_1);
	port->setQueryMode(QextSerialPort::Polling);
}  
void Widget::Connection()
{
	if(isConnection)
	{
		isConnection = false;
		char portByte = 0x7F;
		port->write(&portByte, 1);
		boutonConnection->setText("Connecter");
		port->close();
		return;
	}
    int index = listeCOM->currentIndex();

	
	char buff[256];
	sprintf(buff, "Connection port <%s>",listePorts.at(index).portName.toLocal8Bit().constData());
	QMessageBox::information(this, "port info", buff);
	sprintf(buff, "%s",listePorts.at(index).portName.toLocal8Bit().constData());
	port->setPortName(buff); //we create the port
    port->open(QIODevice::ReadWrite | QIODevice::Unbuffered); //we open the port
    if(!port->isOpen())
    {
        QMessageBox::warning(this, "Erreur port", "Impossible d'ouvrir le port");
    }
	else
	{
		isConnection = true;
		boutonConnection->setText("Déconnecter");
		char portByte = 0x7E;
		port->write(&portByte, 1); // début
	}
	//QMessageBox::information(this, "port info", "OK");
}
void Widget::ActualiserPorts()
{
	listePorts = QextSerialEnumerator::getPorts();

	listeCOM->clear();

	for (int i = 0; i < listePorts.size(); i++)
	{
		
		char buff[64];
		sprintf(buff,"%s", listePorts.at(i).friendName.toLocal8Bit().constData());
		listeCOM->addItem(buff);
	}
}


void Widget::EnvoyerRobotParametres()
{
	if(!isConnection)
		return;

	//QMessageBox::warning(this, "port info", "Envoyer Robot Param");
	
	QString data;
	float acc = float(acceleration->value());
	float dec = float(deceleration->value());
	float rd = float(roueDroite->value());
	float rg = float(roueGauche->value());
	float ec = float(ecartRoue->value());
	float rc = float(roueCodeuse->value());

	char abcd[32];
	char* buff;

	buff = (char*)&acc;
	for(int i=0; i<4; i++)
		abcd[i+1] = buff[i];

	buff = (char*)&dec;
	for(int i=0; i<4; i++)
		abcd[i+5] = buff[i];

	buff = (char*)&rd;
	for(int i=0; i<4; i++)
		abcd[i+9] = buff[i];

	buff = (char*)&rg;
	for(int i=0; i<4; i++)
		abcd[i+13] = buff[i];

	buff = (char*)&ec;
	for(int i=0; i<4; i++)
		abcd[i+17] = buff[i];

	buff = (char*)&rc;
	for(int i=0; i<4; i++)
		abcd[i+21] = buff[i];

	abcd[0] = 0x80;
	port->write(abcd, 24+1);
}
void Widget::EnvoyerPidParametres()
{
	if(!isConnection)
		return;

	//QMessageBox::warning(this, "port info", "Envoyer Pid Param");

	char abcd[64];
	char* buff;

	for(int i=0; i<4; i++)
	{
		for(int j=0; j<3; j++)
		{
			float value = coeffPID[i][j]->value();
			buff = (char*)&value;

			for(int k=0; k<4; k++)
				abcd[4*j + 3*4*i +k+1] = buff[k];   
		}
	}
	abcd[0] = 0x81;
	port->write(abcd, 3*4*4+1);
}
void Widget::EnvoyerCommandeAvancer()
{
	if(!isConnection)
		return;
	//QMessageBox::warning(this, "port info", "Envoyer Commande Avancer");
	float Distance = commandeAvancer->value();
	char abcd[16];
	char* buff = (char*)&Distance;
	for(int i=0; i<4; i++)
		abcd[i+1] = buff[i];

	abcd[0] = 0x01; //numero + ordre
	port->write(abcd,5); //send the buffer
}
void Widget::EnvoyerCommandeTourner()
{
	if(!isConnection)
		return;
	//QMessageBox::warning(this, "port info", "Envoyer Commande Tourner");
	float Angle = (commandeTourner->value())*M_PI/180.f;
	char abcd[8];
	char* buff = (char*)&Angle;
	for(int i=0; i<4; i++)
		abcd[i+1] = buff[i];

	abcd[0] = 0x02; //numero + ordre
	port->write(abcd,5); //send the buffer
}
void Widget::EnvoyerCommandeAller()
{
	//QMessageBox::warning(this, "port info", "Envoyer Commande Aller");
	if(!isConnection)
		return;
	
	//port->write("a"); //send the buffer
	//return;

	float Pos;
	char abcd[16];

	Pos = commandeAller[0]->value();
	char* buff = (char*)&Pos;
	for(int i=0; i<4; i++)
		abcd[i+1] = buff[i];

	Pos = commandeAller[1]->value();
	for(int i=0; i<4; i++)
		abcd[i+5] = buff[i];

	abcd[0] = 0x03; //numero + ordre
	port->write(abcd,9); //send the buffer
}