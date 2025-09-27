#include "../Widgets/ScaledText.h"
#include "../base/View.h"
#include "../util/catch2/catch.hpp"
#include "ControllerTestHelpers.h"

using namespace Ipponboard;
using namespace TestSupport;

namespace
{
QString scaled_text(const ScaledText* widget)
{
    REQUIRE(widget != nullptr);
    return widget->GetText();
}
} // namespace

TEST_CASE("[View] Updates fighter names from controller state")
{
    QApplication& app = ensure_qt_app();
    Q_UNUSED(app);

    ControllerFixture fixture;
    auto& controller = fixture.controller;
    controller.SetFighterName(FighterEnum::First, QStringLiteral("Alice Smith"));
    controller.SetFighterName(FighterEnum::Second, QStringLiteral("Bob Jones"));

    View view(controller.GetIController(), EditionType::Single, View::eTypePrimary);
    controller.RegisterView(&view);

    view.UpdateView();

    auto* lastname_first = view.findChild<ScaledText*>("text_lastname_first");
    auto* firstname_first = view.findChild<ScaledText*>("text_firstname_first");
    auto* lastname_second = view.findChild<ScaledText*>("text_lastname_second");
    auto* firstname_second = view.findChild<ScaledText*>("text_firstname_second");

    INFO(scaled_text(lastname_first).toStdString());
    INFO(scaled_text(firstname_first).toStdString());
    INFO(scaled_text(lastname_second).toStdString());
    INFO(scaled_text(firstname_second).toStdString());
    REQUIRE(scaled_text(lastname_first) == QStringLiteral("JONES"));
    REQUIRE(scaled_text(firstname_first) == QStringLiteral("BOB"));
    REQUIRE(scaled_text(lastname_second) == QStringLiteral("SMITH"));
    REQUIRE(scaled_text(firstname_second) == QStringLiteral("ALICE"));
}

TEST_CASE("[View] Reflects score and timers after controller actions")
{
    QApplication& app = ensure_qt_app();
    Q_UNUSED(app);

    ControllerFixture fixture;
    auto& controller = fixture.controller;
    controller.SetFighterName(FighterEnum::First, QStringLiteral("Alice"));
    controller.SetFighterName(FighterEnum::Second, QStringLiteral("Bob"));

    View view(controller.GetIController(), EditionType::Single, View::eTypePrimary);
    controller.RegisterView(&view);

    controller.SetRoundTime(QTime(0, 0, 10));
    controller.DoAction(eAction_Hajime_Mate);
    controller.AdvanceTimerTicks(eTimer_Main, 3);
    controller.DoAction(eAction_Wazaari, FighterEnum::First);
    controller.DoAction(eAction_Shido, FighterEnum::Second);

    view.UpdateView();

    auto* main_clock = view.findChild<ScaledText*>("text_main_clock");
    auto* wazaari_first = view.findChild<ScaledText*>("text_wazaari_first");
    auto* wazaari_second = view.findChild<ScaledText*>("text_wazaari_second");

    REQUIRE(main_clock != nullptr);
    REQUIRE(wazaari_first != nullptr);
    REQUIRE(wazaari_second != nullptr);

    INFO(scaled_text(main_clock).toStdString());
    INFO(scaled_text(wazaari_first).toStdString());
    REQUIRE(scaled_text(main_clock) == QStringLiteral("0:07"));
    REQUIRE(scaled_text(wazaari_first) == QStringLiteral("0"));
    REQUIRE(scaled_text(wazaari_second) == QStringLiteral("1"));
}
