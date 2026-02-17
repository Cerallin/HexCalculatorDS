#include "config.h"

using namespace HexCalc;

Config HexCalc::config;

HandleEventResult
Config::HandleEvent(const Event &e) {
    HandleEventResult res = Ignored;

    if (e.type == UpdateBaseEvent) {
        auto newBase = UpdateBaseEventData(e.data).newBase;
        base = newBase;
        res = Handled;
    } else if (e.type == UpdateWidthEvent) {
        auto newWidth = UpdateWidthEventData(e.data).newWidth;
        width = newWidth;
        res = Handled;
    } else if (e.type == UpdateSignEvent) {
        auto newSign = UpdateSignEventData(e.data).newSign;
        sign = newSign;
        res = Handled;
    }

    return res;
}
