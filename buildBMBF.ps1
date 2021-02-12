# Builds a .zip file for loading with BMBF
& $PSScriptRoot/build.ps1

if ($?) {
    Compress-Archive -Path "./libs/arm64-v8a/libzenjeqt.so", "./libs/arm64-v8a/libbeatsaber-hook_1_0_12.so", "./bmbfmod.json" -DestinationPath "./zenjeqt_v0.1.0.zip" -Update
}
