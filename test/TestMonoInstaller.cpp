#include "TestMonoInstaller.hpp"
#include "TestBehaviour.hpp"
#include "logger.hpp"

#include "Zenject/ConcreteIdBinderGeneric_1.hpp"

DEFINE_CLASS(Zenjeqt::TestMonoInstaller);

void Zenjeqt::TestMonoInstaller::InstallBindings() {
    LOG_CALLER;
    Container->Bind<Zenjeqt::TestBehaviour*>()->FromNewComponentOnRoot()->AsSingle()->NonLazy();
}
