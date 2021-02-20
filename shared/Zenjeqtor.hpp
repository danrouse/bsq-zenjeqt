#pragma once

#include "InstallBuilder.hpp"
#include "Zenject/IInstaller.hpp"
#include "Zenject/InjectAttribute.hpp"
#include "GlobalNamespace/QuestAppInit.hpp"
#include "GlobalNamespace/GameCoreSceneSetup.hpp"
#include "GlobalNamespace/GameplayCoreInstaller.hpp"
#include "System/Attribute.hpp"
#include "System/Type.hpp"

// Macros for use in place of custom-types' REGISTER_* defaults.
// Registers members as being injectable.
#define REGISTER_FIELD_INJECT(name, ...) \
    REGISTER_FIELD(name); \
    Zenjeqt::Zenjeqtor::RegisterInject(klass, #name, ##__VA_ARGS__);
#define REGISTER_FIELD_INJECT_OPTIONAL(name, ...) \
    REGISTER_FIELD(name); \
    Zenjeqt::Zenjeqtor::RegisterInject(klass, #name, true, ##__VA_ARGS__);
#define REGISTER_METHOD_INJECT(name, ...) \
    REGISTER_METHOD(name); \
    Zenjeqt::Zenjeqtor::RegisterInject(klass, #name, ##__VA_ARGS__);
#define REGISTER_METHOD_INJECT_OPTIONAL(name, ...) \
    REGISTER_METHOD(name); \
    Zenjeqt::Zenjeqtor::RegisterInject(klass, #name, true, ##__VA_ARGS__);

typedef std::unordered_map<const Il2CppClass*, std::unordered_map<std::string, Zenject::InjectAttribute*>> ZenjectInjectMembers;

namespace Zenjeqt {
    /**
     * @brief Direct port of SiraUtil's Zenjector, plus handling for C++ registration.
     * Aside from a being a port of SiraUtil.Zenjector, this class stores references
     * to members of custom-types classes, along with manually-created InjectAttributes.
     * These are looked up later in a hook of System.Attribute.GetCustomAttributes,
     * in order to provide functionality resembling the C# Zenject [Inject*] attributes.
     * References are created by using the REGISTER_*_INJECT macros when creating a
     * custom type, instead of the original custom-types macros.
     */
    class Zenjeqtor {
        public:
        static ZenjectInjectMembers InjectMembers;
        static void RegisterInject(const Il2CppClass* klass, std::string memberName, bool optional, std::string id) {
            static auto InjectAttributeClass = il2cpp_utils::GetClassFromName("Zenject", "InjectAttribute");
            if (!InjectAttributeClass->initialized) il2cpp_functions::Class_Init(InjectAttributeClass);
            auto attribute = CRASH_UNLESS(il2cpp_utils::New<Zenject::InjectAttribute*, il2cpp_utils::CreationType::Manual>());
            attribute->Optional = optional;
            if (id.length() > 0) attribute->Id = il2cpp_utils::createcsstr(id);
            if (!InjectMembers.contains(klass)) InjectMembers[klass] = {};
            InjectMembers[klass][memberName] = attribute;
        }
        static void RegisterInject(const Il2CppClass* klass, std::string memberName, bool optional) {
            RegisterInject(klass, memberName, optional, "");
        }
        static void RegisterInject(const Il2CppClass* klass, std::string memberName) {
            RegisterInject(klass, memberName, false, "");
        }
        template <typename T>
        static void RegisterInject(std::string memberName, bool optional, std::string id) {
            RegisterInject(il2cpp_functions::class_from_system_type(csTypeOf(T)), memberName, optional, id);
        }
        template <typename T>
        static void RegisterInject(std::string memberName, bool optional) {
            RegisterInject(il2cpp_functions::class_from_system_type(csTypeOf(T)), memberName, optional);
        }
        template <typename T>
        static void RegisterInject(std::string memberName) {
            RegisterInject(il2cpp_functions::class_from_system_type(csTypeOf(T)), memberName);
        }


        // The remainder of this class is largely a direct port of SiraUtil's Zenjector class
        static std::vector<Zenjeqtor*> AllZenjeqtors;
        static std::string LastTransitionSetupName;
        static std::string LastGamemodeSetupName;
        static std::string LastMidSceneName;
        static bool ProjectContextWentOff;

        bool Enabled = true;
        std::vector<InstallBuilder*> Builders;

        Zenjeqtor() {
            AllZenjeqtors.push_back(this);
        }

        template <typename T, ::std::enable_if_t<!::std::is_base_of_v<Zenject::IInstaller*, T>, int> = 0>
        InstallBuilder* OnApp() {
            auto ib = new InstallBuilder(csTypeOf(T));
            Builders.push_back(ib);
            ib->On(reinterpret_cast<System::Type*>(csTypeOf(GlobalNamespace::QuestAppInit*))->get_FullName());
            return ib;
        }

        template <typename T, ::std::enable_if_t<!::std::is_base_of_v<Zenject::IInstaller*, T>, int> = 0>
        InstallBuilder* OnMenu() {
            auto ib = new InstallBuilder(csTypeOf(T));
            Builders.push_back(ib);
            ib->On("Menu");
            return ib;
        }

        template <typename T, ::std::enable_if_t<!::std::is_base_of_v<Zenject::IInstaller*, T>, int> = 0>
        InstallBuilder* OnGame(bool onGameSetup) {
            auto ib = new InstallBuilder(csTypeOf(T));
            Builders.push_back(ib);
            ib->On(onGameSetup ?
                reinterpret_cast<System::Type*>(csTypeOf(GlobalNamespace::GameCoreSceneSetup*))->get_FullName() :
                reinterpret_cast<System::Type*>(csTypeOf(GlobalNamespace::GameplayCoreInstaller*))->get_FullName()
            );
            return ib;
        }

        template <typename T, ::std::enable_if_t<!::std::is_base_of_v<Zenject::IInstaller*, T>, int> = 0>
        InstallBuilder* OnGame() {
            return OnGame<T>(true);
        }

        template <typename T, ::std::enable_if_t<!::std::is_base_of_v<Zenject::IInstaller*, T>, int> = 0>
        InstallBuilder* Register() {
            auto ib = new InstallBuilder();
            Builders.push_back(ib);
            return ib->Register<T>();
        }

        template <typename T>
        InstallBuilder* On() {
            return On(reinterpret_cast<System::Type*>(csTypeOf(T))->get_FullName());
        }

        InstallBuilder* On(Il2CppString* destination) {
            return On(to_utf8(csstrtostr(destination)));
        }

        InstallBuilder* On(std::string destination) {
            return OnGeneric(destination);
        }

        // TODO: I'm not actually sure how this works in C# since it appears to call itself forever.
        // InstallBuilder* On(System::Func_4<UnityEngine::SceneManagement::Scene, Zenject::Context*, Zenject::DiContainer*, bool> func) {
        //     auto ib = new InstallBuilder();
        //     Builders.push_back(ib);
        //     return ib.On(func);
        // }

        private:
        InstallBuilder* OnGeneric(std::string destination) {
            auto ib = new InstallBuilder();
            Builders.push_back(ib);
            return ib->On(destination);
        }
    };
}
