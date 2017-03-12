#ifndef CUSTOMLIST_H
#define CUSTOMLIST_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QObject>
/**
* \enum eGC_TYPE
* \brief Differents types of volumes.
*/
typedef enum
{
	eGC_NORMAL,    /*!< Normal Volume type */
	eGC_OTHER,    /*!< Other type of volume */
}
eGC_TYPE;

/**
* \enum e_ALGO
* \brief Differents types of encyption algorithms.
*/
typedef enum
{
	eGC_GOST,    /*!< GOST 28147-89 */
	eGC_GHOPPER,    /*!< GOST grasshopper */
}
eGC_ALGO;

class GC_VolumeList : public QWidget
{
	Q_OBJECT
public:
	GC_VolumeList(
		const QString& volumeLetter,
		const QString& path,
		const eGC_TYPE& type,
		const eGC_ALGO& algorithm,
		const unsigned long long& size,
		const int& width);
	~GC_VolumeList();
signals:
	void Clicked(QString);
protected:
	void mousePressEvent(QMouseEvent* event);
	void GC_VolumeList::enterEvent(QEvent * event);
	void GC_VolumeList::leaveEvent(QEvent * event);
private:
	const QString m_volumeLetter;
	const QString m_path;
	const eGC_TYPE m_type;
	const eGC_ALGO m_algorithm;
	const unsigned long int m_size;
	QLabel* m_bkGray;
	QLabel* m_pathLabel;
	QLabel* m_volumeLetterLabel;
	QLabel* m_sizeLabel;
	QLabel* m_algorithmLabel;
	QLabel* m_typeLabel;
	QLabel* m_diskLabel;
};

#endif // CUSTOMLIST_H
