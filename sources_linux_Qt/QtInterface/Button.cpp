#include "Button.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

Button::Button(QWidget* parent,
                     const QString& text,
                     QString* style,
                     ButtonType type) :
    QLabel(parent)
{

#ifdef QT_DEBUG
    //qDebug() << *style;
#endif

    setText(text);
    setStyleSheet(*style);
    setProperty("pressed", false);

    switch(type)
    {
    case eLeftGreen:
        setObjectName("leftG");
        break;

    case eFullGreen:
        setObjectName("fullG");
        break;

    case eVerif:
        setObjectName("verif");
        break;
    }
}

Button::~Button() {

}

void Button::mousePressEvent(QMouseEvent* event)
{
#ifdef QT_DEBUG
   // qDebug() << event;
#endif
    setProperty("pressed", true);
    style()->unpolish(this);
    style()->polish(this);
    QLabel::setGeometry(QRect(this->x(), this->y()+1, this->width(), this->height()));
}

void Button::mouseReleaseEvent(QMouseEvent* event)
{
#ifdef QT_DEBUG
    //qDebug() << event;
#endif
    setProperty("pressed", false);
    style()->unpolish(this);
    style()->polish(this);
    QLabel::setGeometry(QRect(this->x(), this->y()-1, this->width(), this->height()));
    emit Clicked();
}

void Button::setGeometry(const int & width,
                            const int & height,
                            const int & x,
                            const int & y)
{
    QLabel::setGeometry(QRect(QPoint(x, y),
                            QSize(width, height)));
}
