#include "Installer.hpp"

DEFINE_CLASS(Zenjeqt::Installer);

// Constructor must be explicitly defined
void Zenjeqt::Installer::ctor() {}
// This should never get called (must be stubbed because of vtables of abstract class tree)
void Zenjeqt::Installer::__Unused_InstallBindings() {}
// This method should get overwritten by child installers
void Zenjeqt::Installer::InstallBindings() {}
