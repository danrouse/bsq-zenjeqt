#pragma once

#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Zenjeqt, TestClass, Il2CppObject,
    // Constructor injection: constructor args will be populated
    // when an instance of this class is created by Zenject
    DECLARE_CTOR(ctor, Il2CppString* injectedString);

    REGISTER_FUNCTION(TestClass,
        REGISTER_METHOD(ctor);
    )
)
