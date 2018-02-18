#ifndef DRAGWINDOWPROVIDER_H
#define DRAGWINDOWPROVIDER_H
//https://stackoverflow.com/questions/39088835/dragging-frameless-window-jiggles-in-qml
#include <QObject>
#include <QCursor>

/**
 * @brief
 *
 */
class DragWindowProvider : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief
     *
     * @param parent
     */
    explicit DragWindowProvider(QObject *parent = nullptr) : QObject(parent)
    {
    }
    /**
     * @brief
     *
     */
    virtual ~DragWindowProvider() = default;

    /**
     * @brief
     *
     * @return QPointF
     */
    Q_INVOKABLE QPointF cursorPos()
    {
        return QCursor::pos();
    }
};

#endif // DRAGWINDOWPROVIDER_H
