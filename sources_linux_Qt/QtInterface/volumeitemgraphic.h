//#ifndef VOLUMEITEMGRAPHIC_H
//#define VOLUMEITEMGRAPHIC_H
//#include <QString>
//#include <QList>
//#include <QColor>
//#include <QPainter>
///**
//* \enum VolumeType
//* \brief Differents types of volumes.
//*/
//enum class VolumeType
//{
//    eNormal,    /*!< Normal Volume type */
//    eOther,    /*!< Other type of volume */
//};

///**
//* \enum VolumeAlgorithm
//* \brief Differents types of encyption algorithms.
//*/
//enum class VolumeAlgorithm
//{
//    eGost,    /*!< GOST 28147-89 */
//    eGrasshopper,    /*!< GOST grasshopper */
//};
///**
// * \struct volumeItem
// * \brief Represents a volume entry in the UI list
// */
//struct volumeItem {
//    int mSize;
//    QString mPath;
//    VolumeType mType;
//    VolumeAlgorithm mAlgorithm;
//    /*!
//     * \brief getSize
//     * \return The size of the current volume (byte)
//     */
//    int getSize()
//    {
//        return mSize;
//    }
//    /*!
//     * \brief getPath
//     * \return The volume path
//     */
//    QString getPath() {
//        return mPath;
//    }
//    /*!
//     * \brief getType
//     * \return
//     */
//    VolumeType getType() {
//        return mType;
//    }
//    /*!
//     * \brief getAlgorithm
//     * \return
//     */
//    VolumeAlgorithm getAlgorithm() {
//        return mAlgorithm;
//    }

//};

////TODO : classe de gostcrypt à la place de la classe custom

//class VolumeItemGraphic : public QObject
//{
//    Q_OBJECT
//    //Q_PROPERTY(QString volumePath READ volumePath WRITE setVolumePath)

//public:
//    explicit VolumeItemGraphic(QObject *parent);
//    Q_INVOKABLE void addVolumeItem(int aWidth=0,
//                                   QString aVolumeLetter="",
//                                   QString aPath=""/*,
//                                   VolumeType aType,
//                                   VolumeAlgorithm aAlgorithm*/);
//    Q_INVOKABLE volumeItem getVolumeItem(int aIndex) const;
//   // void paint(QPainter * painter);
//private:
//    QString volumePath;
//    QList<struct volumeItem> volumeList;
//    QColor mBackgroundColor;
//};
//*/
//#endif // VOLUMEITEMGRAPHIC_H
