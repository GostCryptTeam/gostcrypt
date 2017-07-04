#include "volumecreation.h"


VolumeCreation::VolumeCreation()
{

}

void VolumeCreation::createVolume()
{
     mVCreator.CreateVolume(
                 shared_ptr<GostCrypt::VolumeCreationOptions>(&mInfos));
}


template<typename T>
void VolumeCreation::setInfo(const QString &aKey, T aValue)
{
   switch(aKey.toStdString())
   {
   case "Path":
       mInfos.Path = aValue; //VolumePath
       break;
   case "Type":
       mInfos.Type =  aValue; //GostCrypt::VolumeType::Enum
       break;
   case "Size":
       mInfos.Size =  aValue; //uint64
       break;
   case "Password":
       mInfos.Password =  shared_ptr<GostCrypt::VolumePassword>(&aValue);
       break;
   case "Keyfiles":
       mInfos.Keyfiles =  shared_ptr<GostCrypt::KeyfileList>(&aValue);
       break;
   case "VolumeHeaderKdf":
       mInfos.VolumeHeaderKdf =  shared_ptr<GostCrypt::Pkcs5Kdf>(&aValue);
       break;
   case "EA":
       mInfos.EA =  shared_ptr<GostCrypt::EncryptionAlgorithm>(&aValue);
       break;
   case "Quick":
       mInfos.Quick =  aValue; //bool
       break;
   case "Filesystem":
       mInfos.Filesystem =  aValue; //FilesystemType::Enum
       break;
   case "FilesystemClusterSize":
       mInfos.FilesystemClusterSize =  aValue; //uint32
       break;
   case "SectorSize":
       mInfos.SectorSize =  aValue; //uint32
       break;
   }
}

