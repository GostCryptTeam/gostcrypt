/*#include "volumeitemgraphic.h"

VolumeItemGraphic::VolumeItemGraphic(QObject *parent) :
    QObject(parent),
    mBackgroundColor(255,255,255)
{

}

void VolumeItemGraphic::addVolumeItem(int aSize,
                                      QString aVolumeLetter,
                                      QString aPath,
                                      VolumeType aType,
                                      VolumeAlgorithm aAlgorithm)
{
    struct volumeItem volume;
    volume.mAlgorithm = aAlgorithm;
    volume.mType = mType;
    volume.mPath = aPath;
   // volume.mVolumeLetter = aVolumeLetter;
    volume.mSize = aSize;
    volumeList.append(volume);
}

volumeItem VolumeItemGraphic::getVolumeItem(int aIndex) const
{
    return volumeList.at(aIndex);
}

void VolumeItemGraphic::paint(QPainter *painter)
{
    QRectF rectangle(0.0, 0.0, 575.0, 80.0);
    painter->set(mBackgroundColor);
    paint.drawRect(rectangle);
}
*/
