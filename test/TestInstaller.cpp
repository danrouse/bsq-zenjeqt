#include "logger.hpp"
#include "TestInstaller.hpp"
#include "TestClass.hpp"

DEFINE_CLASS(Zenjeqt::TestInstaller);

// Constructor MUST be explicitly defined on basic Installer types
void Zenjeqt::TestInstaller::ctor() {}

void Zenjeqt::TestInstaller::InstallBindings() {
    LOG_CALLER;
    container->Bind<Il2CppString*>()->FromInstance(il2cpp_utils::createcsstr("Hello world!"));
    container->Bind<Zenjeqt::TestClass*>()->AsSingle()->NonLazy();
}
