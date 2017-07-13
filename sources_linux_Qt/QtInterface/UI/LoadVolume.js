function loadVolume(MountPoint, EncryptionAlgorithmName, Path, Size) {
    console.log("Ajout du volume");
    if(helpStart.visible == true)
    {
        helpStart.visible = false
    }

    listOfVolumes.append({MountPoint_: MountPoint, EncryptionAlgorithmName_: EncryptionAlgorithmName, Path_: Path, Size_: Size})
}
