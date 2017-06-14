function loadVolume(MountPoint, EncryptionAlgorithmName, Path, Size) {
    console.log("Ajout du volume");
    listOfVolumes.append({MountPoint_: MountPoint, EncryptionAlgorithmName_: EncryptionAlgorithmName, Path_: Path, Size_: Size})
}
