#ifndef DRAGWINDOWPROVIDER_H
#define DRAGWINDOWPROVIDER_H
//https://stackoverflow.com/questions/39088835/dragging-frameless-window-jiggles-in-qml
#include <QObject>
#include <QCursor>

class DragWindowProvider : public QObject
{
    Q_OBJECT
public:
    explicit DragWindowProvider(QObject *parent = nullptr) : QObject(parent)
    {
    }
    virtual ~DragWindowProvider() = default;

    Q_INVOKABLE QPointF cursorPos()
    {
        return QCursor::pos();
    }
};

#endif // DRAGWINDOWPROVIDER_H
