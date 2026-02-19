#include "config.h"

using namespace HexCalc;

Config HexCalc::config;

EventResult
Config::HandleEvent(const Event &e) {
    EventResult res = Skipped;

    if (e.type == UpdateBaseEvent) {
        auto newBase = UpdateBaseEventData(e.data).newBase;
        base = newBase;
        res = Consumed;
    } else if (e.type == UpdateWidthEvent) {
        auto newWidth = UpdateWidthEventData(e.data).newWidth;
        width = newWidth;
        res = Consumed;
    } else if (e.type == UpdateSignEvent) {
        auto newSign = UpdateSignEventData(e.data).newSign;
        sign = newSign;
        res = Consumed;
    }

    return res;
}
