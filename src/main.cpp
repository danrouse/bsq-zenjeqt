#include "main.hpp"
#include "logger.hpp"
#include "hooks.hpp"
#include "test.hpp"
#include "Zenjeqtor.hpp"

Logger& getLogger() {
    static Logger* logger = new Logger({ID, VERSION});
    return *logger;
}

extern "C" void setup(ModInfo& info) {}

extern "C" void load() {
    il2cpp_functions::Init();
    Zenjeqt::InstallHooks();
    ZenjeqtTest::DoTheThing();
}
