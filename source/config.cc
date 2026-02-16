#include "config.h"

using namespace HexCalc;

Config HexCalc::config;

HandleEventResult
Config::HandleEvent(const Event &e) {
    if (e.type != UpdateConfigEvent) {
        return Ignored;
    }

    UpdateConfigEventData data = e.data;

    auto newBase = data.newBase;
    auto newWidth = data.newWidth;
    auto newSign = data.newSign;

    base = newBase;
    width = newWidth;
    sign = newSign;

    return Handled;
}
