mkdir github_release
& .\buildBMBF.ps1
mv zenjeqt_v0.1.1.zip github_release
cp libs\arm64-v8a\libzenjeqt.so github_release
cp obj\local\arm64-v8a\libzenjeqt.so github_release\libzenjeqt_debug.so
