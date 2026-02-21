#include "config.h"

using namespace HexCalc;

ConfigModel HexCalc::config;

EventResult
ConfigModel::HandleEvent(const Event &e) {
    EventResult res = Skipped;

    if (e.type == UpdateBaseEvent) {
        base = static_cast<NumberBase>(e.data);
        res = Consumed;
    } else if (e.type == UpdateWidthEvent) {
        width = static_cast<NumberWidth>(e.data);
        res = Consumed;
    } else if (e.type == UpdateSignEvent) {
        sign = static_cast<NumberSign>(e.data);
        res = Consumed;
    }

    return res;
}
