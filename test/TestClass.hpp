#pragma once

#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Zenjeqt, TestClass, Il2CppObject,
    DECLARE_CTOR(ctor, Il2CppString* injectedString);
    REGISTER_FUNCTION(TestClass,
        REGISTER_METHOD(ctor);
    )
)
