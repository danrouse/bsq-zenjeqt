#include "TestBehaviour.hpp"
#include "logger.hpp"

DEFINE_CLASS(Zenjeqt::TestBehaviour);

void Zenjeqt::TestBehaviour::Awake() {
    LOG_DEBUG("MainMenuViewController at %lp", _MainMenuViewController);
}
void Zenjeqt::TestBehaviour::OnEnable() {
    LOG_DEBUG("MainMenuViewController at %lp", _MainMenuViewController);
}
void Zenjeqt::TestBehaviour::Start() {
    LOG_DEBUG("MainMenuViewController at %lp", _MainMenuViewController);
}

// void Zenjeqt::TestBehaviour::InjectMe(GlobalNamespace::MainMenuViewController* MainMenuViewController) {
//     LOG_DEBUG("MainMenuViewController at %lp", MainMenuViewController);
// }
