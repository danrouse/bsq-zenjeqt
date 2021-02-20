#pragma once

#include "logger.hpp"
#include "Zenjeqtor.hpp"
#include "Zenject/Context.hpp"
#include "Zenject/GameObjectContext.hpp"
#include "Zenject/SceneContext.hpp"
#include "Zenject/SceneDecoratorContext.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/InstallerBase.hpp"
#include "Zenject/MonoInstaller.hpp"
#include "Zenject/ScriptableObjectInstaller.hpp"
#include "Zenject/ConcreteIdBinderNonGeneric.hpp"
#include "Zenject/ZenjectException.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "System/Action.hpp"
#include "System/Reflection/MemberTypes.hpp"
#include "GlobalNamespace/SceneInfo.hpp"

namespace Zenjeqt {
    void InstallHooks();
}
