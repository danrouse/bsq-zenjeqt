#pragma once

#ifdef ZENJEQT_DEBUG
#include "TestInstaller.hpp"
#include "TestMonoInstaller.hpp"
#include "TestBehaviour.hpp"
#include "TestClass.hpp"
#include "custom-types/shared/register.hpp"

namespace ZenjeqtTest {
    inline void DoTheThing() {
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
#else
namespace ZenjeqtTest {
    inline void DoTheThing() {}
}
#endif
