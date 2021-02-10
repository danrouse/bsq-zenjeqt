#include "TestBehaviour.hpp"
#include "logger.hpp"

DEFINE_CLASS(Zenjeqt::TestBehaviour);

void Zenjeqt::TestBehaviour::Start() {
    LOG_DEBUG("Field-injected MainMenuViewController at %lp", _MainMenuViewController);
}
void Zenjeqt::TestBehaviour::InjectMe(GlobalNamespace::MainFlowCoordinator* MainFlowCoordinator) {
    LOG_DEBUG("Method-injected MainFlowCoordinator at %lp", MainFlowCoordinator);
}
