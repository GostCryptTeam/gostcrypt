#include "GC_button.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

GC_button::GC_button(QWidget* parent,
                     const QString& text,
                     QString* style,
                     eGC_buttonType type) :
    QLabel(parent)
{

#ifdef QT_DEBUG
    qDebug() << *style;
#endif

    setText(text);
    setStyleSheet(*style);
    setProperty("pressed", false);

    switch(type)
    {
    case eGC_leftGreen:
        setObjectName("leftG");
        break;

    case eGC_fullGreen:
        setObjectName("fullG");
        break;

    case eGC_verif:
        setObjectName("verif");
        break;
    }
}

GC_button::~GC_button() {

}

void GC_button::mousePressEvent(QMouseEvent* event)
{
#ifdef QT_DEBUG
    qDebug() << event;
#endif
    setProperty("pressed", true);
    style()->unpolish(this);
    style()->polish(this);
    QLabel::setGeometry(QRect(this->x(), this->y()+1, this->width(), this->height()));
}

void GC_button::mouseReleaseEvent(QMouseEvent* event)
{
#ifdef QT_DEBUG
    qDebug() << event;
#endif
    setProperty("pressed", false);
    style()->unpolish(this);
    style()->polish(this);
    QLabel::setGeometry(QRect(this->x(), this->y()-1, this->width(), this->height()));
    emit Clicked();
}

void GC_button::setGeometry(const int & width,
                            const int & height,
                            const int & x,
                            const int & y)
{
    QLabel::setGeometry(QRect(QPoint(x, y),
                            QSize(width, height)));
}
