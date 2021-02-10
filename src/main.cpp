#include "main.hpp"
#include "logger.hpp"
#include "hooks.hpp"
#include "Zenjeqtor.hpp"
#ifdef ZENJEQT_DEBUG
#include "test.hpp"
#endif

Logger& getLogger() {
    static Logger* logger = new Logger({ID, VERSION});
    return *logger;
}

extern "C" void setup(ModInfo& info) {}

extern "C" void load() {
    il2cpp_functions::Init();
    Zenjeqt::InstallHooks();
    #ifdef ZENJEQT_DEBUG
    Zenjeqt::Test();
    #endif
}
