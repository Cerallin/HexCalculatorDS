#include "config.h"

using namespace HexCalc;

Config HexCalc::config;

HandleEventResult
Config::HandleEvent(const Event &e) {
    return Handled;
}
