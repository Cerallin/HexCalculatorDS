/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "event.h"
#include "structure.h"

namespace HexCalc {

CircularQueue<Event, EventQueueSize> eventQueue;

}
