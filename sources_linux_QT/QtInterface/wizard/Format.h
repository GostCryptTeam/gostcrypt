#ifndef FORMAT_H
#define FORMAT_H
#include "Ui_CreationWizard.h"

/*!
 * \fn void Ui_Creation::Format_Options()
 * \brief seventh step of creation : The Filesystem type (FAT,EXT etc.)
 *
 */
void Format_Options();
/*!
 * \fn void Ui_Creation::Volume_Format()
 * \brief eigth step of creation : Format the volume
 *
 */
void Volume_Format();
/*!
 * \fn Ui_Creation::End()
 * \brief last step of creation : The final page
 *
 */
void End();

#endif // FORMAT_H
