#pragma once

#include "shared/Zenjeqtor.hpp"
#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"
#include "GlobalNamespace/MainFlowCoordinator.hpp"

DECLARE_CLASS_CODEGEN(Zenjeqt, TestBehaviour, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(GlobalNamespace::MainMenuViewController*, _MainMenuViewController);
    DECLARE_METHOD(void, InjectMe, GlobalNamespace::MainFlowCoordinator* MainFlowCoordinator);
    DECLARE_METHOD(void, Start);
    REGISTER_FUNCTION(TestBehaviour,
        // Field injection: Zenject will populate this field when creating an
        // instance of this class
        REGISTER_FIELD_INJECT(_MainMenuViewController);

        // Method injection: Zenject will run this method and populate its arguments
        // when creating an instance of this class
        REGISTER_METHOD_INJECT(InjectMe);

        // For MonoBehaviours, the Zenject dependency graph will be fully populated,
        // with all dependencies injected, by the time Start is called by Unity
        REGISTER_METHOD(Start);
    )
)
