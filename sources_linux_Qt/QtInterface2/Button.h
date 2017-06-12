#ifndef BUTTON_H
#define BUTTON_H

#include <QStyle>
#include <QLabel>
#include <QMainWindow>
#include <QString>
#include <QMouseEvent>
#include <QPixmap>
#include <QPainter>

/**
 * \enum ButtonType
 * \brief Differents types of buttons for GostCrypt.
 */
typedef enum
{
   eLeftGreen,    /*!< Button with a left-icon */
   eFullGreen,    /*!< Button with full background */
   eVerif         /*!< Verficiation buttons */
}
ButtonType;


class Button : public QLabel
{
    Q_OBJECT
public:
       explicit Button(QWidget* parent,
                 const QString& text,
                 QString * style,
                 ButtonType type = eFullGreen
                 );
       ~Button();

       virtual void setGeometry(const int & width,
                                const int & height,
                                const int & x,
                                const int & y);
signals:
    void Clicked();
protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
};

#endif // BUTTON_H
