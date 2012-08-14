#include "MyLabel.h"

MyLabel::MyLabel():QLabel()
{}

MyLabel::MyLabel(QWidget *parent, Qt::WindowFlags f):QLabel()
{}

void MyLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        emit clicked(event->x(),event->y());
    }
}