#ifndef mGC_BUTTON_H
#define mGC_BUTTON_H

#include <QStyle>
#include <QLabel>
#include <QMainWindow>
#include <QString>
#include <QMouseEvent>
#include <QPixmap>
#include <QPainter>

/**
 * \enum eGC_buttonType
 * \brief Differents types of buttons for GostCrypt.
 */
typedef enum
{
   eGC_leftGreen,    /*!< Button with a left-icon */
   eGC_fullGreen,    /*!< Button with full background */
}
eGC_buttonType;


class GC_button : public QLabel
{
    Q_OBJECT
public:
       explicit GC_button(QWidget* parent,
                 const QString& text,
                 QString * style,
                 eGC_buttonType type = eGC_fullGreen
                 );
       ~GC_button();

       virtual void setGeometry(const int & width,
                                const int & height,
                                const int & x,
                                const int & y);
signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
};

#endif // mGC_BUTTON_H
