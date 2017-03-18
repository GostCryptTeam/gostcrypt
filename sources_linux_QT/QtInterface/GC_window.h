/*!
 * \file mGC_window.h
 * \brief Class for GostCrypt's UI
 * \author Antoine Hébert, Louis Béclair, William Lardier
 * \version 0.1
 * \date 15/07/2017
 *
 * GUI for GostCrypt 2.0
 *
 */

#ifndef mGC_WINDOW_H
#define mGC_WINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QDesktopWidget>
#include <QLabel>
#include <QString>
#include <QPushButton>
#include <QFile>
#include <QFontDatabase>
#include <QFont>
#include <QGridLayout>
#include <QFontMetrics>
#include <QFrame>
#include <QGroupBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QComboBox>
#include <QAbstractScrollArea>
#include <QMap>
#include <QListWidget>
#include <QTableWidget>
#include <QStandardItemModel>
#include <QScrollArea>

#include "GC_button.h"
#include "GC_volumelist.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

#define UI_RATIO 1.41
#define HEIGHT 530
#define WIDTH HEIGHT*UI_RATIO

namespace GC_UI {
	class GC_window;
};

	class GC_window : public QMainWindow
	{
		Q_OBJECT

	public:
		//! Constructor
		explicit GC_window(/*QWidget *parent = 0*/);
		//! Destructor
		~GC_window();

	protected :
		void ResizeEvent(QResizeEvent* event);
		void CreateVolumeWidget(
			const QString& volumeLetter,
			const QString& path,
			const eGC_TYPE& type,
			const eGC_ALGO& algorithm,
			const unsigned long long& size
		);

	private slots:
		void DebugSlot(QString path);

	private:

		int mGC_width; /*!< Main Window's width */
		int mGC_height; /*!< Main Window's height */
		int mGC_gridCpt; /*!< Main Window's height */

		QLabel * mGC_logo; /*!< Label where is draw the logo */
		QLabel mGC_sidebar; /*!< left-sidebar with white background */
		QLabel mGC_title; /*!< "GostCrypt volume creation Wizard" */
		QLabel * mGC_textHistory; /*!< "Never save history" */
		QLabel * mGC_lock; /*!< Green lock in 'mGC_Volume' group */

		GC_button * mGC_newVolume; /*!< sends signal to create a new volume */
		GC_button * mGC_selectFile; /*!< sends signal to select a new volume file */
		GC_button * mGC_volumeTools; /*!< sends signal to open the volume tools window */
		GC_button * mGC_selectDevice; /*!< sends signal to let the user to choose the device */
		GC_button * mVolumeProperties; /*!< Opens window with volume information */
		GC_button * mWipeCache; /*!< TODO */
		GC_button * mMount; /*!< Mounts selected volume */
		GC_button * mAutoMount; /*!< Mounts all loaded volumes */
		GC_button * mDismount;  /*!< Dismount a selected volume */
		GC_button * mDismountAll; /*!< Dismount all volumes */

		QString mGC_styleSheet; /*!< stylesheet */
		QFont font; /*!< text font */
		QFrame * mGC_separator; /*!< gray line */

		QGroupBox * mGC_Volume; /*!< Volume section */
		QGroupBox * mGC_ButtonVolume; /*!< Volume section */
		QGroupBox * mGC_ButtonMount; /*!< Mount volume section */
		QCheckBox * mGC_saveHistory; /*!< Allow user (or not) to save the history
										 of previously loaded path(s) */
		QGridLayout * mGC_grid; /*!< Grid that contains volume groupbox elements */
		QGridLayout * mGC_ButtonGrid; /*!< TODO */
		QGridLayout * mGC_ButtonMountGrid; /*!< TODO */
		QComboBox * mGC_volumePath; /*!< Grid that contains volume groupbox elements */

		QGroupBox * mGC_VolumeProperty; /*!< Volume property section */
		QMap<QString, GC_VolumeList*> mGC_VolumeItems; /*!< All Widget Volume items */
		QVBoxLayout * mGC_VolumesWidget; /*!< Widget with all volumes */
		QWidget* mGC_VolumeWidget;
		QGridLayout* mGC_VolumeLayout;
		QScrollArea* mGC_VolumeScroll;
	};


#endif // mGC_WINDOW_H
