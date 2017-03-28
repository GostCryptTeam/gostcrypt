#ifndef GRAPHICUSERINTERFACE_H
#define GRAPHICUSERINTERFACE_H

#include <QObject>

class GraphicUserInterface : public QObject
{
    Q_OBJECT
public:
    explicit GraphicUserInterface(QObject *parent = 0);

public slots:
    void receive(const QString&);
    void receiveMount(const QString&, const QString&);
    void receiveAutoMount();
    void receiveDismountAll();
};

#endif // GRAPHICUSERINTERFACE_H
