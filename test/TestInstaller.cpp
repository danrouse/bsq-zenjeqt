#include "TestInstaller.hpp"
#include "TestClass.hpp"
#include "logger.hpp"

DEFINE_CLASS(Zenjeqt::TestInstaller);

void Zenjeqt::TestInstaller::ctor() {}

// This should never get called (must be stubbed because of vtables of abstract class tree)
void Zenjeqt::TestInstaller::__Unused_InstallBindings() {}

void Zenjeqt::TestInstaller::InstallBindings() {
    LOG_CALLER;
    container->Bind<Il2CppString*>()->FromInstance(il2cpp_utils::createcsstr("Hello world!"));
    container->Bind<Zenjeqt::TestClass*>()->AsSingle()->NonLazy();
}
