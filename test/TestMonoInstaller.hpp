#pragma once

#include "custom-types/shared/macros.hpp"
#include "MonoInstaller.hpp"

DECLARE_CLASS_CODEGEN(Zenjeqt, TestMonoInstaller, Zenjeqt::MonoInstaller,
    DECLARE_OVERRIDE_METHOD(void, InstallBindings, il2cpp_utils::FindMethodUnsafe("Zenjeqt", "MonoInstaller", "InstallBindings", 0));

    REGISTER_FUNCTION(TestMonoInstaller,
        REGISTER_METHOD(InstallBindings);
    )
)
