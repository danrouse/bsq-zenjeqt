#pragma once

#include "custom-types/shared/macros.hpp"
#include "Zenject/Installer.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/ConcreteIdBinderGeneric_1.hpp"

DECLARE_CLASS_CODEGEN(Zenjeqt, Installer, Zenject::Installer,
    DECLARE_CTOR(ctor);
    DECLARE_OVERRIDE_METHOD(void, __Unused_InstallBindings, il2cpp_utils::FindMethodUnsafe("Zenject", "IInstaller", "InstallBindings", 0));
    DECLARE_OVERRIDE_METHOD(void, InstallBindings, il2cpp_utils::FindMethodUnsafe("Zenject", "InstallerBase", "InstallBindings", 0));

    REGISTER_FUNCTION(TestInstaller,
        REGISTER_METHOD(ctor);
        REGISTER_METHOD(__Unused_InstallBindings);
        REGISTER_METHOD(InstallBindings);
    )
)
