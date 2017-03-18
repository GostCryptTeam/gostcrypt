#ifndef GC_GRAPHICUSERINTERFACE_H
#define GC_GRAPHICUSERINTERFACE_H

#include <QObject>

class GC_GraphicUserInterface : public QObject
{
    Q_OBJECT
public:
    explicit GC_GraphicUserInterface(QObject *parent = 0);

public slots:
    void GC_receive(const QString&);
    void GC_receiveMount();
    void GC_receiveAutoMount();
    void GC_receiveDismountAll();
};

#endif // GC_GRAPHICUSERINTERFACE_H
