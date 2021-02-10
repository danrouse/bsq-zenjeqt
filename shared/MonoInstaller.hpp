#pragma once

#include "custom-types/shared/macros.hpp"
#include "Zenject/MonoInstaller.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/ConcreteIdBinderGeneric_1.hpp"

DECLARE_CLASS_CODEGEN(Zenjeqt, MonoInstaller, Zenject::MonoInstaller,
    DECLARE_OVERRIDE_METHOD(void, InstallBindings, il2cpp_utils::FindMethodUnsafe("Zenject", "IInstaller", "InstallBindings", 0));

    REGISTER_FUNCTION(TestMonoInstaller,
        REGISTER_METHOD(InstallBindings);
    )
)
