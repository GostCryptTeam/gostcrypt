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
#include "GC_graphicuserinterface.h"
#include "GC_openvolume.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

#define UI_RATIO 1.41
#define HEIGHT 530
#define WIDTH HEIGHT*UI_RATIO

namespace GC_UI {
	class GC_window;
}

	class GC_window : public QMainWindow
	{
		Q_OBJECT

	public:
		//! Constructor
		explicit GC_window(/*QWidget *parent = 0*/);
		//! Destructor
		~GC_window();

    public slots:
        // void AddVolume(); //! TODO

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
        void OpenVolume();

	private:
        void ConnectSignals();

		int mGC_width; /*!< Main Window's width */
		int mGC_height; /*!< Main Window's height */
		int mGC_gridCpt; /*!< Main Window's height */

		QLabel * mGC_logo; /*!< Label where is draw the logo */
		QLabel mGC_sidebar; /*!< left-sidebar with white background */
		QLabel mGC_title; /*!< "GostCrypt volume creation Wizard" */
		QLabel * mGC_textHistory; /*!< "Never save history" */
		QLabel * mGC_lock; /*!< Green lock in 'mGC_Volume' group */

		GC_button * mGC_newVolume; /*!< sends signal to create a new volume */
        GC_button * mGC_Mount; /*!< Mounts selected volume */
        GC_button * mGC_AutoMount; /*!< Mounts all loaded volumes */
        GC_button * mGC_DismountAll; /*!< Dismount all volumes */
        GC_button * mGC_OpenVolume; /*!< Open an existing volume */

		QString mGC_styleSheet; /*!< stylesheet */
        QFont mGC_font; /*!< text font */
		QFrame * mGC_separator; /*!< gray line */

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
        QWidget * mGC_VolumeWidget;
        QGridLayout * mGC_VolumeLayout;
        QScrollArea * mGC_VolumeScroll;

        GC_GraphicUserInterface mGC_slots; /*!< Slot manager */
        GC_OpenVolume * mGC_OpenVolumeWindow; /*!< Dialog to open a volume file */
	};


#endif // mGC_WINDOW_H
