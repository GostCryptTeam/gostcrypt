#ifndef MODELCLASS_H
#define MODELCLASS_H

#include <QQuickItem>
#include <QString>
#include <QStringList>

class ModelClass : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString test READ getText NOTIFY testChanged)

public:
    ModelClass(QQuickItem* parent=0):QQuickItem(parent){}
    ~ModelClass(){}

    QString getText() const;
    Q_INVOKABLE void setText(const QString& txt);

signals:
    void testChanged();

public:
    QString _test;
};


#endif
