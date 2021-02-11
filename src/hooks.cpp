#include "hooks.hpp"

::Array<System::Attribute*>* InsertCustomAttribute(::Array<System::Attribute*>* arr, System::Attribute* attribute) {
    auto newArray = ::Array<System::Attribute*>::NewLength(arr->Length() + 1);
    il2cpp_utils::RunMethodUnsafe(arr, "System.Collections.Generic.ICollection`1.CopyTo", newArray, 0);
    newArray->values[arr->Length()] = reinterpret_cast<System::Attribute*>(attribute);
    return newArray;
}

/**
 * @brief System.Attribute.GetCustomAttributes hook (overload 1 of 2).
 * Override attribute checks in order to search C++ metadata for custom types.
 * This works around the current inability to define actual C# attributes.
 * This hook handles fields.
 */
MAKE_HOOK_OFFSETLESS(Attribute_GetCustomAttributes_Member, ::Array<System::Attribute*>*, System::Reflection::MemberInfo* element, System::Type* type, bool inherit) {
    auto baseValue = Attribute_GetCustomAttributes_Member(element, type, inherit);

    auto declaringType = CRASH_UNLESS(il2cpp_utils::RunMethod<System::Type*>(element, "get_DeclaringType"));
    auto declaringClass = il2cpp_functions::class_from_system_type(reinterpret_cast<Il2CppReflectionType*>(declaringType));
    auto memberName = to_utf8(csstrtostr(CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(element, "get_Name"))));
    if (Zenjeqt::Zenjeqtor::InjectMembers.contains({declaringClass, memberName})) {
        baseValue = InsertCustomAttribute(baseValue, Zenjeqt::Zenjeqtor::InjectMembers[{declaringClass, memberName}]);
    }
    return baseValue;
}

/**
 * @brief System.Attribute.GetCustomAttributes hook (overload 2 of 2).
 * Override attribute checks in order to search C++ metadata for custom types.
 * This works around the current inability to define actual C# attributes.
 * This hook handles properties.
 */
// TODO: Parameter injection NYI, and _may_ not ever be.
// It is be possible but would either end up working very badly,
// or require fairly large changes to how custom types are created.
// MAKE_HOOK_OFFSETLESS(Attribute_GetCustomAttributes_Parameter, ::Array<System::Attribute*>*, System::Reflection::ParameterInfo* element, System::Type* type, bool inherit) {
//     return Attribute_GetCustomAttributes_Parameter(element, type, inherit);
// }

/**
 * @brief MonoMethod.GetCustomAttributes hook.
 * Override attribute checks in order to search C++ metadata for custom types.
 * This works around the current inability to define actual C# attributes.
 * This hook handles methods.
 */
MAKE_HOOK_OFFSETLESS(MonoMethod_GetCustomAttributes, ::Array<::Il2CppObject*>*, System::Reflection::MonoMethod* self, System::Type* attributeType, bool inherit) {
    auto baseValue = MonoMethod_GetCustomAttributes(self, attributeType, inherit);
    static auto InjectAttributeBaseClass = il2cpp_utils::GetClassFromName("Zenject", "InjectAttributeBase");
    auto attributeClass = il2cpp_functions::class_from_system_type(reinterpret_cast<Il2CppReflectionType*>(attributeType));
    if (attributeClass != InjectAttributeBaseClass) return baseValue;

    auto declaringClass = il2cpp_functions::class_from_system_type(reinterpret_cast<Il2CppReflectionType*>(self->get_DeclaringType()));
    auto memberName = to_utf8(csstrtostr(self->get_Name()));
    if (Zenjeqt::Zenjeqtor::InjectMembers.contains({declaringClass, memberName})) {
        baseValue = reinterpret_cast<::Array<Il2CppObject*>*>(
            InsertCustomAttribute(reinterpret_cast<::Array<System::Attribute*>*>(baseValue), Zenjeqt::Zenjeqtor::InjectMembers[{declaringClass, memberName}])
        );
    }
    return baseValue;
}

/**
 * @brief Zenject.Context.InstallInstallers hook.
 * The main entry point, largely ported from SiraUtil.
 * Registers all InstallBuilders' Installers with actual Zenject and calls explicit initializers.
 */
MAKE_HOOK_OFFSETLESS(Zenject_InstallInstallers, void, Zenject::Context* self,
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
            Zenject_InstallInstallers(self, normalInstallers, normalInstallerTypes, scriptableObjectInstallers, installers, installerPrefabs);
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

    Zenject_InstallInstallers(self, normalInstallers, normalInstallerTypes, scriptableObjectInstallers, installers, installerPrefabs);
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
MAKE_HOOK_OFFSETLESS(ZenjectException_1, void, Zenject::ZenjectException* self, Il2CppString* message) {
    getLogger().error("ZenjectException: " + to_utf8(csstrtostr(message)));
    ZenjectException_1(self, message);
}
MAKE_HOOK_OFFSETLESS(ZenjectException_2, void, Zenject::ZenjectException* self, Il2CppString* message, System::Exception* innerException) {
    getLogger().error("ZenjectException: " + to_utf8(csstrtostr(innerException->message)));
    ZenjectException_2(self, message, innerException);
}

void Zenjeqt::InstallHooks() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), Zenject_InstallInstallers, il2cpp_utils::FindMethodUnsafe("Zenject", "Context", "InstallInstallers", 5));
    // INSTALL_HOOK_OFFSETLESS(getLogger(), Attribute_GetCustomAttributes_Parameter, il2cpp_utils::FindMethod("System", "Attribute", "GetCustomAttributes",
    //     std::vector<Il2CppClass*>{},
    //     std::vector<const Il2CppType*>{
    //         il2cpp_functions::class_get_type(classof(System::Reflection::ParameterInfo*)),
    //         il2cpp_functions::class_get_type(classof(System::Type*)),
    //         il2cpp_functions::class_get_type(il2cpp_functions::defaults->boolean_class)
    //     }
    // ));
    INSTALL_HOOK_OFFSETLESS(getLogger(), Attribute_GetCustomAttributes_Member, il2cpp_utils::FindMethod("System", "Attribute", "GetCustomAttributes",
        std::vector<Il2CppClass*>{},
        std::vector<const Il2CppType*>{
            il2cpp_functions::class_get_type(classof(System::Reflection::MemberInfo*)),
            il2cpp_functions::class_get_type(classof(System::Type*)),
            il2cpp_functions::class_get_type(il2cpp_functions::defaults->boolean_class)
        }
    ));
    INSTALL_HOOK_OFFSETLESS(getLogger(), MonoMethod_GetCustomAttributes, il2cpp_utils::FindMethodUnsafe("System.Reflection", "MonoMethod", "GetCustomAttributes", 2));
    INSTALL_HOOK_OFFSETLESS(getLogger(), GameScenesManager_PushScenes, il2cpp_utils::FindMethodUnsafe("", "GameScenesManager", "PushScenes", 4));
    INSTALL_HOOK_OFFSETLESS(getLogger(), ZenjectException_1, il2cpp_utils::FindMethodUnsafe("Zenject", "ZenjectException", ".ctor", 1));
    INSTALL_HOOK_OFFSETLESS(getLogger(), ZenjectException_2, il2cpp_utils::FindMethodUnsafe("Zenject", "ZenjectException", ".ctor", 2));
}
