#pragma once

#include "TestInstaller.hpp"
#include "TestMonoInstaller.hpp"
#include "TestBehaviour.hpp"
#include "TestClass.hpp"
#include "custom-types/shared/register.hpp"

#include "System/Reflection/BindingFlags.hpp"

namespace Zenjeqt {
    inline void Test() {
        custom_types::Register::RegisterTypes<
            Zenjeqt::TestBehaviour,
            Zenjeqt::TestClass,
            Zenjeqt::TestInstaller,
            Zenjeqt::TestMonoInstaller
        >();
        auto zenjeqtor = new Zenjeqt::Zenjeqtor();
        zenjeqtor->OnApp<Zenjeqt::TestInstaller*>();
        zenjeqtor->OnMenu<Zenjeqt::TestMonoInstaller*>();
    }
}
