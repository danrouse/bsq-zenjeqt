#pragma once

#include "logger.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "System/Type.hpp"
#include "System/Func_1.hpp"
#include "System/Func_4.hpp"
#include "System/Action_1.hpp"
#include "System/Action_2.hpp"
#include "Zenject/Context.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/SceneContext.hpp"
#include "Zenject/IInstaller.hpp"
#include "ModestTree/_Assert.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/MissionLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/MultiplayerLevelScenesTransitionSetupDataSO.hpp"

typedef std::function<void(Zenject::DiContainer*, Zenject::SceneDecoratorContext*, std::vector<UnityEngine::MonoBehaviour*>, UnityEngine::MonoBehaviour*)> ZenjeqtMutator;

namespace Zenjeqt {
    /**
     * @brief Direct port of SiraUtil's InstallBuilder.
     * This class provides chainable methods for setting up an Installer.
     */
    class InstallBuilder {
        public:
        System::Type* Type = nullptr;
        std::string Destination = "";
        // object[] Parameters
        System::Func_1<bool>* WhenInstall = nullptr;
        std::unordered_set<std::string> Circuits = {};
        System::Action_1<Zenject::DiContainer*>* Contextless = nullptr;
        std::unordered_set<System::Type*> Exposers = {};
        System::Action_2<Zenject::SceneContext*, Zenject::DiContainer*>* Resolved = nullptr;
        System::Action_2<Zenject::Context*, Zenject::DiContainer*>* SceneContextless = nullptr;
        std::vector<std::pair<System::Type*, ZenjeqtMutator>> Mutators = {};
        std::unordered_set<System::Func_4<UnityEngine::SceneManagement::Scene, Zenject::Context*, Zenject::DiContainer*, bool>*> OnFuncs = {};
        // HashSet<Tuple<Type, Action<Context, DiContainer>>> Headers

        InstallBuilder() {}
        InstallBuilder(System::Type* type) {
            Type = type;
        }
        InstallBuilder(Il2CppReflectionType* type) {
            Type = reinterpret_cast<System::Type*>(type);
        }

        // InstallBuilder* WithParameters(params object[] parameters) {
        //     Parameters = parameters;
        //     return this;
        // }

        InstallBuilder* On(System::Func_4<UnityEngine::SceneManagement::Scene, Zenject::Context*, Zenject::DiContainer*, bool>* func) {
            OnFuncs.insert(func);
            return this;
        }

        InstallBuilder* On(std::string destination) {
            LOG_DEBUG(destination);
            Destination = destination;
            return this;
        }

        InstallBuilder* On(Il2CppString* destination) {
            return On(to_utf8(csstrtostr(destination)));
        }

        template <typename T>
        InstallBuilder* On() {
            return On(reinterpret_cast<System::Type*>(csTypeOf(T))->get_Name());
        }

        template <typename T, ::std::enable_if_t<!::std::is_base_of_v<Zenject::IInstaller*, T>, int> = 0>
        InstallBuilder* Register() {
            Type = reinterpret_cast<System::Type*>(csTypeOf(T));
            return this;
        }

        template <typename T>
        InstallBuilder* ShortCircuitFor() {
            return ShortCircuitFor(reinterpret_cast<System::Type*>(csTypeOf(T))->get_Name());
        }

        InstallBuilder* ShortCircuitForStandard() {
            return ShortCircuitFor(reinterpret_cast<System::Type*>(csTypeOf(GlobalNamespace::StandardLevelScenesTransitionSetupDataSO*))->get_FullName());
        }

        InstallBuilder* ShortCircuitForCampaign() {
            return ShortCircuitFor(reinterpret_cast<System::Type*>(csTypeOf(GlobalNamespace::MissionLevelScenesTransitionSetupDataSO*))->get_FullName());
        }

        InstallBuilder* ShortCircuitForTutorial() {
            return ShortCircuitFor("Tutorial");
        }

        InstallBuilder* ShortCircuitForMultiplayer() {
            return ShortCircuitFor(reinterpret_cast<System::Type*>(csTypeOf(GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO*))->get_FullName());
        }

        InstallBuilder* ShortCircuitFor(Il2CppString* shortCircuiter) {
            return ShortCircuitFor(to_utf8(csstrtostr(shortCircuiter)));
        }

        InstallBuilder* ShortCircuitFor(std::string shortCircuiter) {
            Circuits.insert(shortCircuiter);
            return this;
        }

        InstallBuilder* OnlyForStandard() {
            ShortCircuitForCampaign();
            ShortCircuitForTutorial();
            ShortCircuitForMultiplayer();
            return this;
        }

        InstallBuilder* OnlyForMultiplayer() {
            ShortCircuitForCampaign();
            ShortCircuitForTutorial();
            ShortCircuitForStandard();
            return this;
        }

        template <typename T, ::std::enable_if_t<!::std::is_base_of_v<UnityEngine::MonoBehaviour*, T>, int> = 0>
        InstallBuilder* Expose() {
            Exposers.insert(csTypeOf(T));
        }

        template <typename T, ::std::enable_if_t<!::std::is_base_of_v<UnityEngine::MonoBehaviour*, T>, int> = 0>
        InstallBuilder* Mutate(ZenjeqtMutator mutator) {
            Mutators.emplace_back({csTypeOf(T), mutator});
        }

        InstallBuilder* Pseudo(System::Action_1<Zenject::DiContainer*>* action) {
            Contextless = action;
            return this;
        }

        InstallBuilder* Pseudo(System::Action_2<Zenject::Context*, Zenject::DiContainer*>* action) {
            SceneContextless = action;
            return this;
        }

        InstallBuilder* Initialized(System::Action_2<Zenject::SceneContext*, Zenject::DiContainer*>* onInit) {
            Resolved = onInit;
            return this;
        }

        InstallBuilder* When(System::Func_1<bool>* when) {
            WhenInstall = when;
            return this;
        }

        void Validate() {
            if (Contextless == nullptr && SceneContextless == nullptr) {
                static auto IInstallerType = il2cpp_utils::GetSystemType("Zenject", "IInstaller");
                CRASH_UNLESS(Type != nullptr);
                ModestTree::_Assert::DerivesFrom(Type, IInstallerType);
            }
            CRASH_UNLESS(Destination.length() > 0);
        }

        // internal class DelegateWrapper
        // {
        //     public Action<MutationContext, object> actionObj;

        //     public DelegateWrapper Wrap<T, U>(Action<T, U> callback) where T : MutationContext
        //     {
        //         actionObj = delegate (MutationContext context, object obj)
        //         {
        //             callback((T)context, (U)obj);
        //         };
        //         return this;
        //     }
        // }
    };
}
