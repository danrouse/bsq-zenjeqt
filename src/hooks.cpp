#include "hooks.hpp"

/**
 * @brief Inserts a System.Attribute into an array of Attributes.
 */
::Array<System::Attribute*>* InsertCustomAttribute(::Array<System::Attribute*>* arr, System::Attribute* attribute) {
    auto newArray = ::Array<System::Attribute*>::NewLength(arr->Length() + 1);
    il2cpp_utils::RunMethodUnsafe(arr, "System.Collections.Generic.ICollection`1.CopyTo", newArray, 0);
    newArray->values[arr->Length()] = reinterpret_cast<System::Attribute*>(attribute);
    return newArray;
}

/**
 * @brief Checks if a type derives from Zenject.InjectAttributeBase.
 */
bool DerivesFromInjectAttributeBase(System::Type* type) {
    static auto InjectAttributeBaseType = il2cpp_utils::GetSystemType("Zenject", "InjectAttributeBase");
    return type == InjectAttributeBaseType || type->IsAssignableFrom(InjectAttributeBaseType);
}

/**
 * @brief Get an InjectAttribute registered with an arbitrary member.
 */
std::optional<System::Attribute*> GetInjectAttribute(System::Reflection::MemberInfo* member) {
    auto declaringType = RET_NULLOPT_UNLESS(getLogger(), il2cpp_utils::RunMethod<Il2CppReflectionType*>(member, "get_DeclaringType"));
    auto declaringClass = il2cpp_functions::class_from_system_type(declaringType);
    if (!Zenjeqt::Zenjeqtor::InjectMembers.contains(declaringClass)) return std::nullopt;
    auto memberNameCs = RET_NULLOPT_UNLESS(getLogger(), il2cpp_utils::RunMethod<Il2CppString*>(member, "get_Name"));
    auto memberName = to_utf8(csstrtostr(memberNameCs));
    if (!Zenjeqt::Zenjeqtor::InjectMembers[declaringClass].contains(memberName)) return std::nullopt;
    return Zenjeqt::Zenjeqtor::InjectMembers[declaringClass][memberName];
}

/**
 * @brief MonoField.GetCustomAttributes hook.
 * Override attribute checks on a field in order to search C++ metadata for custom types.
 */
MAKE_HOOK_OFFSETLESS(MonoField_GetCustomAttributes, ::Array<::Il2CppObject*>*, System::Reflection::MonoField* self, System::Type* attributeType, bool inherit) {
    auto baseValue = MonoField_GetCustomAttributes(self, attributeType, inherit);
    if (!DerivesFromInjectAttributeBase(attributeType)) return baseValue;
    auto attribute = RET_UNLESS(baseValue, getLogger(), GetInjectAttribute(self));
    return reinterpret_cast<::Array<Il2CppObject*>*>(
        InsertCustomAttribute(reinterpret_cast<::Array<System::Attribute*>*>(baseValue), attribute)
    );
}

/**
 * @brief MonoMethod.GetCustomAttributes hook.
 * Override attribute checks on a method in order to search C++ metadata for custom types.
 */
MAKE_HOOK_OFFSETLESS(MonoMethod_GetCustomAttributes, ::Array<::Il2CppObject*>*, System::Reflection::MonoMethod* self, System::Type* attributeType, bool inherit) {
    auto baseValue = MonoMethod_GetCustomAttributes(self, attributeType, inherit);
    if (!DerivesFromInjectAttributeBase(attributeType)) return baseValue;
    auto attribute = RET_UNLESS(baseValue, getLogger(), GetInjectAttribute(self));
    return reinterpret_cast<::Array<Il2CppObject*>*>(
        InsertCustomAttribute(reinterpret_cast<::Array<System::Attribute*>*>(baseValue), attribute)
    );
}

/**
 * @brief Zenject.Context.InstallInstallers hook.
 * The main entry point, largely ported from SiraUtil.
 * Registers all InstallBuilders' Installers with actual Zenject and calls explicit initializers.
 */
MAKE_HOOK_OFFSETLESS(Context_InstallInstallers, void, Zenject::Context* self,
    System::Collections::Generic::List_1<Zenject::InstallerBase*>* normalInstallers,
    System::Collections::Generic::List_1<System::Type*>* normalInstallerTypes,
    System::Collections::Generic::List_1<Zenject::ScriptableObjectInstaller*>* scriptableObjectInstallers,
    System::Collections::Generic::List_1<Zenject::MonoInstaller*>* installers,
    System::Collections::Generic::List_1<Zenject::MonoInstaller*>* installerPrefabs
) {
    // port of Harmony patch (SiraUtil.Zenject.HarmonyPatches.SiraContextDecorator.Prefix)
    std::vector<Il2CppString*> contractNames;
    std::vector<Zenject::SceneDecoratorContext*> decoratorContexts;

    static auto SceneContextClass = il2cpp_utils::GetClassFromName("Zenject", "SceneContext");
    static auto SceneDecoratorContextClass = il2cpp_utils::GetClassFromName("Zenject", "SceneDecoratorContext");
    if (self->klass == SceneContextClass) {
        auto sceneContext = reinterpret_cast<Zenject::SceneContext*>(self);
        for (int i = 0; i < sceneContext->contractNames->get_Count(); i++) {
            contractNames.push_back(sceneContext->contractNames->get_Item(i));
        }
        for (int i = 0; i < sceneContext->decoratorContexts->get_Count(); i++) {
            decoratorContexts.push_back(sceneContext->decoratorContexts->get_Item(i));
        }
    }
    if (self->klass == SceneDecoratorContextClass) {
        decoratorContexts.push_back(reinterpret_cast<Zenject::SceneDecoratorContext*>(self));
    }

    std::unordered_set<std::string> sourceNames;
    for (auto& contractName : contractNames) sourceNames.insert(to_utf8(csstrtostr(contractName)));
    for (int i = 0; i < installerPrefabs->get_Count(); i++) sourceNames.insert(to_utf8(csstrtostr(installerPrefabs->get_Item(i)->GetType()->get_FullName())));
    for (int i = 0; i < installers->get_Count(); i++) sourceNames.insert(to_utf8(csstrtostr(installers->get_Item(i)->GetType()->get_FullName())));
    for (int i = 0; i < normalInstallers->get_Count(); i++) sourceNames.insert(to_utf8(csstrtostr(normalInstallers->get_Item(i)->GetType()->get_FullName())));
    for (int i = 0; i < normalInstallerTypes->get_Count(); i++) sourceNames.insert(to_utf8(csstrtostr(normalInstallerTypes->get_Item(i)->get_FullName())));
    for (int i = 0; i < scriptableObjectInstallers->get_Count(); i++) sourceNames.insert(to_utf8(csstrtostr(scriptableObjectInstallers->get_Item(i)->GetType()->get_FullName())));
    for (auto decoratorContext : decoratorContexts) sourceNames.insert(to_utf8(csstrtostr(decoratorContext->get_gameObject()->get_scene().get_name())));

    // Original Harmony patch then calls SiraEvents.SendInstallEvent,
    // which creates SiraEvents.SceneContextInstalledArgs and calls ContextInstalling.Invoke.
    // Next is a port of SiraUtil.Zenject.ZenjectManager.SiraEvents_PreInstall,
    // which is added as an event handler for ContextInstalling.

    // if (sourceNames.contains("AppCore")) {
    //     // "this" is ZenjectManager
    //     // self->get_Container()->BindInstance(this)->AsSingle();
    // }
    if (!Zenjeqt::Zenjeqtor::ProjectContextWentOff) {
        if (sourceNames.contains("AppCore")) {
            Zenjeqt::Zenjeqtor::ProjectContextWentOff = true;
        } else {
            Context_InstallInstallers(self, normalInstallers, normalInstallerTypes, scriptableObjectInstallers, installers, installerPrefabs);
            return;
        }
    }

    // Imagine having LINQ...
    std::vector<Zenjeqt::InstallBuilder*> builders;
    for (auto zenjector : Zenjeqt::Zenjeqtor::AllZenjeqtors) {
        if (!zenjector->Enabled) continue;
        for (auto builder : zenjector->Builders) {
            bool isAllOnFuncsInvoked = true;
            for (auto onFunc : builder->OnFuncs) {
                if (!onFunc->Invoke(self->get_gameObject()->get_scene(), self, self->get_Container())) {
                    isAllOnFuncsInvoked = false;
                }
            }
            if (!isAllOnFuncsInvoked) continue;
            if (!sourceNames.contains(builder->Destination)) continue;
            if (builder->Circuits.contains(Zenjeqt::Zenjeqtor::LastTransitionSetupName)) continue;
            if (builder->Circuits.contains(Zenjeqt::Zenjeqtor::LastGamemodeSetupName)) continue;
            if (builder->Circuits.contains(Zenjeqt::Zenjeqtor::LastMidSceneName)) continue;
            bool hasShortCircuit = false;
            for (auto sourceName : sourceNames) {
                if (builder->Circuits.contains(sourceName)) {
                    hasShortCircuit = true;
                    break;
                }
            }
            if (!hasShortCircuit) {
                builders.push_back(builder);
            }
        }
    }

    std::vector<UnityEngine::MonoBehaviour*> allInjectables;
    for (auto decoratorContext : decoratorContexts) {
        auto injectables = decoratorContext->injectableMonoBehaviours;
        for (int i = 0; i < injectables->get_Count(); i++) {
            allInjectables.push_back(injectables->get_Item(i));
        }
    }

    static auto GameObjectContextClass = il2cpp_utils::GetClassFromName("Zenject", "GameObjectContext");
    if (self->klass == GameObjectContextClass) {
        auto monoList = CRASH_UNLESS(il2cpp_utils::New<System::Collections::Generic::List_1<UnityEngine::MonoBehaviour*>*>());
        reinterpret_cast<Zenject::GameObjectContext*>(self)->GetInjectableMonoBehaviours(monoList);
        for (int i = 0; i < monoList->get_Count(); i++) {
            allInjectables.push_back(monoList->get_Item(i));
        }
    }

    for (auto builder : builders) {
        getLogger().info(
            "Installing: " +
            (builder->Type->get_Namespace() ? to_utf8(csstrtostr(builder->Type->get_Namespace())) : "GlobalNamespace") + "::" +
            to_utf8(csstrtostr(builder->Type->get_Name()))
        );
        builder->Validate();
        if (builder->WhenInstall) {
            if (!builder->WhenInstall->Invoke()) continue;
        }
        if (builder->Contextless) builder->Contextless->Invoke(self->get_Container());
        if (builder->SceneContextless) builder->SceneContextless->Invoke(self, self->get_Container());
        // TODO: Mutators NYI
        // for (auto mutator : builder->Mutators) {
        //     if (!allInjectables.Any(x => x.GetType() == mutator.Item1))
        //     {
        //         Assert.CreateException($"Could not find an object to mutate in a decorator context. {Utilities.ASSERTHIT}", mutator.Item1);
        //     }
        //     var behaviour = allInjectables.FirstOrDefault(x => x.GetType() == mutator.Item1);
        //     if (behaviour != null)
        //     {
        //         var activeDecorator = decoratorContexts.FirstOrDefault(x => Accessors.Injectables(ref x).Contains(behaviour));
        //         mutator.Item2.actionObj.Invoke(new MutationContext(e.Container, activeDecorator, allInjectables), behaviour);
        //     }
        // }
        for (auto exposable : builder->Exposers) {
            UnityEngine::MonoBehaviour* behaviour;
            for (auto& injectable : allInjectables) {
                if (injectable->GetType() == exposable) {
                    behaviour = injectable;
                    break;
                }
            }
            if (!behaviour) continue;
            if (!self->get_Container()->HasBinding(behaviour->GetType())) {
                self->get_Container()->Bind(exposable)->FromInstance(behaviour)->AsSingle();
            }
        }
        if (builder->Resolved != nullptr && self->klass == SceneContextClass) {
            auto sceneContext = reinterpret_cast<Zenject::SceneContext*>(self);
            System::Action* delegate;
            std::function<void()> callback = [&]() {
                sceneContext->remove_PostResolve(delegate);
                builder->Resolved->Invoke(sceneContext, sceneContext->get_Container());
            };
            delegate = il2cpp_utils::MakeDelegate<System::Action*, void>(classof(System::Action*), callback);
            sceneContext->add_PostResolve(delegate);
        }
        if (!builder->Type) continue;
        // TODO: Parameter binding NYI
        // if (builder->Parameters != NULL) {
        //     auto bases = self->get_NormalInstallers();
        //     // Configurable Mono Installers requires the Unity Inspector
        //     // Assert.That(!builder.Type.DerivesFrom<MonoInstallerBase>(), $"MonoInstallers cannot have parameters due to Zenject limitations. {Utilities.ASSERTHIT}");
        //     bases->Add(reinterpret_cast<Zenject::InstallerBase*>(self->get_Container()->Instantiate(builder->Type, builder->Parameters)));
        //     self->set_NormalInstallers(bases);
        //     continue;
        // }
        if (builder->Type->IsSubclassOf(reinterpret_cast<System::Type*>(csTypeOf(Zenject::MonoInstallerBase*)))) {
            installers->Add(reinterpret_cast<Zenject::MonoInstaller*>(self->get_gameObject()->AddComponent(builder->Type)));
            continue;
        }
        if (builder->Type->IsSubclassOf(reinterpret_cast<System::Type*>(csTypeOf(Zenject::InstallerBase*))) && (true/*builder->Parameters == NULL || builder->Parameters->Length == 0)*/)) {
            normalInstallerTypes->Add(builder->Type);
        }
    }

    Context_InstallInstallers(self, normalInstallers, normalInstallerTypes, scriptableObjectInstallers, installers, installerPrefabs);
}

/**
 * @brief GameScenesManager.PushScenes hook.
 * Tracks scene and game mode names.
 * Port of SiraUtil.Zenject.HarmonyPatches.GamemodeDecorator
 */
MAKE_HOOK_OFFSETLESS(GameScenesManager_PushScenes, void, GlobalNamespace::GameScenesManager* self, GlobalNamespace::ScenesTransitionSetupDataSO* scenesTransitionSetupData, float minDuration, Il2CppObject* afterMinDurationCallback, Il2CppObject* finishCallback) {
    Zenjeqt::Zenjeqtor::LastTransitionSetupName = to_utf8(csstrtostr(scenesTransitionSetupData->GetType()->get_Name()));
    auto gameModeProperty = scenesTransitionSetupData->GetType()->GetProperty(il2cpp_utils::createcsstr("gameMode"));
    if (gameModeProperty) {
        Zenjeqt::Zenjeqtor::LastGamemodeSetupName = to_utf8(csstrtostr(
            reinterpret_cast<Il2CppString*>(gameModeProperty->GetValue(scenesTransitionSetupData, nullptr))
        ));
    }
    if (scenesTransitionSetupData->scenes->Length() == 3) {
        Zenjeqt::Zenjeqtor::LastMidSceneName = to_utf8(csstrtostr(scenesTransitionSetupData->scenes->values[1]->sceneName));
    }
    GameScenesManager_PushScenes(self, scenesTransitionSetupData, minDuration, afterMinDurationCallback, finishCallback);
}

/**
 * @brief ZenjectException constructor hooks.
 * Hook constructor of Zenject.ZenjectException to be able to log out the captured inner exception message.
 */
MAKE_HOOK_OFFSETLESS(ZenjectException_ctor_1, void, Zenject::ZenjectException* self, Il2CppString* message) {
    getLogger().error("ZenjectException: " + to_utf8(csstrtostr(message)));
    ZenjectException_ctor_1(self, message);
}
MAKE_HOOK_OFFSETLESS(ZenjectException_ctor_2, void, Zenject::ZenjectException* self, Il2CppString* message, System::Exception* innerException) {
    getLogger().error("ZenjectException: " + to_utf8(csstrtostr(innerException->message)));
    ZenjectException_ctor_2(self, message, innerException);
}

void Zenjeqt::InstallHooks() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), Context_InstallInstallers, il2cpp_utils::FindMethodUnsafe("Zenject", "Context", "InstallInstallers", 5));
    INSTALL_HOOK_OFFSETLESS(getLogger(), MonoField_GetCustomAttributes, il2cpp_utils::FindMethodUnsafe("System.Reflection", "MonoField", "GetCustomAttributes", 2));
    INSTALL_HOOK_OFFSETLESS(getLogger(), MonoMethod_GetCustomAttributes, il2cpp_utils::FindMethodUnsafe("System.Reflection", "MonoMethod", "GetCustomAttributes", 2));
    INSTALL_HOOK_OFFSETLESS(getLogger(), GameScenesManager_PushScenes, il2cpp_utils::FindMethodUnsafe("", "GameScenesManager", "PushScenes", 4));
    INSTALL_HOOK_OFFSETLESS(getLogger(), ZenjectException_ctor_1, il2cpp_utils::FindMethodUnsafe("Zenject", "ZenjectException", ".ctor", 1));
    INSTALL_HOOK_OFFSETLESS(getLogger(), ZenjectException_ctor_2, il2cpp_utils::FindMethodUnsafe("Zenject", "ZenjectException", ".ctor", 2));
}
