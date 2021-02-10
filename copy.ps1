& $PSScriptRoot/build.ps1 --debug
if ($?) {
    adb push libs/arm64-v8a/libzenjeqt.so /sdcard/Android/data/com.beatgames.beatsaber/files/mods/libzenjeqt.so
    if ($?) {
        & $PSScriptRoot/restart-game.ps1
        if ($args[0] -eq "--log") {
            & $PSScriptRoot/start-logging.ps1
        }
    }
}
