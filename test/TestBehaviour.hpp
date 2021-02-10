#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "Zenjeqtor.hpp"

DECLARE_CLASS_CODEGEN(Zenjeqt, TestBehaviour, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(GlobalNamespace::MainMenuViewController*, _MainMenuViewController);
    // DECLARE_METHOD(void, InjectMe, GlobalNamespace::MainMenuViewController* MainMenuViewController);
    DECLARE_METHOD(void, Awake);
    DECLARE_METHOD(void, OnEnable);
    DECLARE_METHOD(void, Start);
    REGISTER_FUNCTION(TestBehaviour,
        REGISTER_FIELD_INJECT(_MainMenuViewController);
        // REGISTER_METHOD(InjectMe);
        REGISTER_METHOD(Awake);
        REGISTER_METHOD(OnEnable);
        REGISTER_METHOD(Start);
    )
)
