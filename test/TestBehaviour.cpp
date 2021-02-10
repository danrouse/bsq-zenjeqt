#include "TestBehaviour.hpp"
#include "logger.hpp"

DEFINE_CLASS(Zenjeqt::TestBehaviour);

void Zenjeqt::TestBehaviour::Start() {
    LOG_DEBUG("MainMenuViewController at %lp", _MainMenuViewController);
}
