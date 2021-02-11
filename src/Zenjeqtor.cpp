#include "Zenjeqtor.hpp"

// Initialize all static members of Zenjeqt::Zenjeqtor
ZenjectInjectMembers Zenjeqt::Zenjeqtor::InjectMembers = {};
std::vector<Zenjeqt::Zenjeqtor*> Zenjeqt::Zenjeqtor::AllZenjeqtors = {};
std::string Zenjeqt::Zenjeqtor::LastTransitionSetupName = "";
std::string Zenjeqt::Zenjeqtor::LastGamemodeSetupName = "";
std::string Zenjeqt::Zenjeqtor::LastMidSceneName = "";
bool Zenjeqt::Zenjeqtor::ProjectContextWentOff = false;
