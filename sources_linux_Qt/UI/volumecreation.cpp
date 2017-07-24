#include "volumecreation.h"


VolumeCreation::VolumeCreation()
{

}

void VolumeCreation::createVolume()
{
     /*mVCreator.CreateVolume(
                 shared_ptr<GostCrypt::VolumeCreationOptions>(&mInfos));*/
}

QStringList VolumeCreation::getAlgos() const
{
    QStringList algos;
    //TODO : récupérer la liste des algos dans GostCrypt::...
    algos.append("GOST 28147-89");
    algos.append("GOST Grasshopper");
    return algos;
}

QStringList VolumeCreation::getHashs() const
{
    QStringList algos;
    //TODO : récupérer la liste des hashs dans GostCrypt::...
    algos.append("GOST R 34.11-2012");
    algos.append("GOST R 34.11-94");
    algos.append("Whirlpool");
    return algos;
}

QString VolumeCreation::getAlgosDescription(const int &aIndex)
{
    switch(aIndex){
    case 0:
        return "The GOST 28147-89 block cipher is part of the Russian cryptographic"
               "standard algorithms. It is a symmetric key block cipher based on a"
               "balanced Feistel network.<br><font color='719c24'><a href='#'>More information on GOST 28147-89</a></font>";
        break;
    case 1:
        return "No description here.";
        break;
    default:
        return "";
    }
}

QString VolumeCreation::getfreeSpace()
{
    return "c++ value TODO"; //mVCreator. TODO

}


template<typename T>
void VolumeCreation::setInfo(const QString &aKey, T aValue)
{
   switch(aKey.toStdString())
   {
   case "Path":
       //mInfos.Path = aValue; //VolumePath
       break;
   case "Type":
       //mInfos.Type =  aValue; //GostCrypt::VolumeType::Enum
       break;
   case "Size":
       //mInfos.Size =  aValue; //uint64
       break;
   case "Password":
       //mInfos.Password =  shared_ptr<GostCrypt::VolumePassword>(&aValue);
       break;
   case "Keyfiles":
       //mInfos.Keyfiles =  shared_ptr<GostCrypt::KeyfileList>(&aValue);
       break;
   case "VolumeHeaderKdf":
       //mInfos.VolumeHeaderKdf =  shared_ptr<GostCrypt::Pkcs5Kdf>(&aValue);
       break;
   case "EA":
       //mInfos.EA =  shared_ptr<GostCrypt::EncryptionAlgorithm>(&aValue);
       break;
   case "Quick":
       //mInfos.Quick =  aValue; //bool
       break;
   case "Filesystem":
       //mInfos.Filesystem =  aValue; //FilesystemType::Enum
       break;
   case "FilesystemClusterSize":
       //mInfos.FilesystemClusterSize =  aValue; //uint32
       break;
   case "SectorSize":
       //mInfos.SectorSize =  aValue; //uint32
       break;
   }
}

