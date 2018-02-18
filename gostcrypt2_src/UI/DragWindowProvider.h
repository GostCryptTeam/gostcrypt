#ifndef DRAGWINDOWPROVIDER_H
#define DRAGWINDOWPROVIDER_H
//https://stackoverflow.com/questions/39088835/dragging-frameless-window-jiggles-in-qml
#include <QObject>
#include <QCursor>

/**
 * @brief
 * This class allows to move the UI in a more fluid way
 * than if it were managed in Javascript from the interface, since it is "borderless".
 */
class DragWindowProvider : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructor who initiates the class.
     * @param parent Parent object
     */
    explicit DragWindowProvider(QObject *parent = nullptr) : QObject(parent)
    {
    }
    /**
     * @brief class destructor
     *
     */
    virtual ~DragWindowProvider() = default;

    /**
     * @brief Function that can be called from the QML to retrieve the cursor position, and thus move the interface accordingly
     *
     * @return QPointF Returns the cursor position to the screen
     */
    Q_INVOKABLE QPointF cursorPos()
    {
        return QCursor::pos();
    }
};

#endif // DRAGWINDOWPROVIDER_H
