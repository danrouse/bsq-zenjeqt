#include "main.hpp"
#include "logger.hpp"
#include "hooks.hpp"
#include "Zenjeqtor.hpp"
#include "Installer.hpp"
#include "MonoInstaller.hpp"
#include "custom-types/shared/register.hpp"
#ifdef ZENJEQT_DEBUG
#include "test.hpp"
#endif

Logger& getLogger() {
    static Logger* logger = new Logger({ID, VERSION});
    return *logger;
}

extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
}

extern "C" void load() {
    il2cpp_functions::Init();
    custom_types::Register::RegisterTypes<Zenjeqt::Installer, Zenjeqt::MonoInstaller>();
    Zenjeqt::InstallHooks();

    #ifdef ZENJEQT_DEBUG
    Zenjeqt::Test();
    #endif
}
