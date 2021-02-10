#include "TestClass.hpp"
#include "logger.hpp"

DEFINE_CLASS(Zenjeqt::TestClass);

void Zenjeqt::TestClass::ctor(Il2CppString* injectedString) {
    LOG_DEBUG("Constructor-injected string: " + to_utf8(csstrtostr(injectedString)));
}
