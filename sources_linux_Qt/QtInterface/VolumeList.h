#ifndef CUSTOMLIST_H
#define CUSTOMLIST_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QObject>
#include <QMouseEvent>
#include "GraphicUserInterface.h"

/**
* \enum VolumeType
* \brief Differents types of volumes.
*/
enum class VolumeType
{
	eNormal,    /*!< Normal Volume type */
	eOther,    /*!< Other type of volume */
};

/**
* \enum VolumeAlgorithm
* \brief Differents types of encyption algorithms.
*/
enum class VolumeAlgorithm
{
	eGost,    /*!< GOST 28147-89 */
	eGrasshopper,    /*!< GOST grasshopper */
};

class VolumeList : public QWidget
{
	Q_OBJECT
public:
	VolumeList(
        GraphicUserInterface * eSlots,
		const QString& volumeLetter,
		const QString& path,
		const VolumeType& type,
		const VolumeAlgorithm& algorithm,
		const unsigned long long& size,
		const int& width);
	~VolumeList();
signals:
	void Clicked(QString);
    void emitDismount(QString);

protected:
	void mousePressEvent(QMouseEvent* event);
    void enterEvent(QEvent * event);
    void leaveEvent(QEvent * event);
private:
    const int mWidth;
	const QString mVolumeLetter;
	const QString mPath;
	const VolumeType mType;
	const VolumeAlgorithm mAlgorithm;
	const unsigned long int mSize;
	QLabel* mBkGray;
	QLabel* mPathLabel;
	QLabel* mVolumeLetterLabel;
	QLabel* mSizeLabel;
	QLabel* mAlgorithmLabel;
	QLabel* mTypeLabel;
	QLabel* mDiskLabel;
    QLabel* mDismountLabel;

    GraphicUserInterface * mSlots;
};

#endif // CUSTOMLIST_H
