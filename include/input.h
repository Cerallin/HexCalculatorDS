/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "commands.h"
#include "common.h"

namespace HexCalc {

struct Point {
    int16_t x;
    int16_t y;

    constexpr Point(int16_t px, int16_t py) : x(px), y(py) {}

    constexpr Point(int data)
        : x(static_cast<int16_t>(data >> 16)),
          y(static_cast<int16_t>(data & 0xFFFF)) {}

    int
    ToInt() const {
        return (static_cast<int>(x) << 16) | static_cast<int>(y);
    }
};

struct Area {
    int16_t x;
    int16_t y;
    uint8_t w;
    uint8_t h;

    Area(Point p1, Point p2) {
        auto x_min = std::min(p1.x, p2.x);
        auto x_max = std::max(p1.x, p2.x);
        auto y_min = std::min(p1.y, p2.y);
        auto y_max = std::max(p1.y, p2.y);

        assert(x_max - x_min <= std::numeric_limits<uint8_t>::max());
        assert(y_max - y_min <= std::numeric_limits<uint8_t>::max());

        auto width = static_cast<uint8_t>(x_max - x_min);
        auto height = static_cast<uint8_t>(y_max - y_min);

        x = x_min;
        y = y_min;
        w = width;
        h = height;
    }

    constexpr Area(int16_t px, int16_t py, uint8_t width, uint8_t height)
        : x(px), y(py), w(width), h(height) {}
};

/**
 * @brief Area space for 6x8 font glyphs. Each unit corresponds to a 6x8 pixel
 * area on the screen, which is the size of a single glyph.
 *
 */
struct Area6x8 {
    int8_t x;
    int8_t y;
    uint8_t w;
    uint8_t h;

    explicit Area6x8(Area area)
        : x(area.x / 6), y(area.y / 8), w(area.w / 6),
          h(area.h / (lineHeight * 8)) {}

    static constexpr uint8_t lineHeight = 2;
};

struct Area8x8 {
    int8_t x;
    int8_t y;
    uint8_t w;
    uint8_t h;

    explicit Area8x8(Area area)
        : x(area.x / 8), y(area.y / 8), w(area.w / 8),
          h(area.h / (lineHeight * 8)) {}

    static constexpr uint8_t lineHeight = 2;
};

/**
 * @brief The InputHandler class is for reading inputs from the keys and the
 * touch screen.
 *
 */
class InputHandler {
  public:
    InputHandler(EventBus &eventBus, Commands &commands)
        : eventBus(eventBus), commands(commands) {}

    void Update(void);

    bool KeyDown(uint32_t k) const;

    bool KeyUp(uint32_t k) const;

    bool TouchDown() const;

    bool TouchUp() const;

  private:
    EventBus &eventBus;
    Commands &commands;

    uint32_t currentKeys = 0;
    uint32_t previousKeys = 0;

    bool stablePressed = false;
    bool previousTouch = false;

    Point smoothPos{0, 0};

    int pressCount = 0;
    int releaseCount = 0;

    static constexpr int PRESS_TH = 2;
    static constexpr int RELEASE_TH = 3;

    void updateKeys(void);
    void updateTouch();

    void notifyTouch(const Point &pos);
};

enum ButtonType : uint8_t {
    ButtonInvalid = 0xFF,
    Button0 = 0,
    Button1,
    Button2,
    Button3,
    Button4,
    Button5,
    Button6,
    Button7,
    Button8,
    Button9,
    ButtonA,
    ButtonB,
    ButtonC,
    ButtonD,
    ButtonE,
    ButtonF,
    ButtonPlus,
    ButtonMinus,
    ButtonMultiply,
    ButtonDivide,
    ButtonLBrac,
    ButtonRBrac,
    ButtonAnd,
    ButtonOr,
    ButtonModulo,
    ButtonLShift,
    ButtonRShift,
    ButtonNegate,
    ButtonBitwiseNot,
    ButtonClear,
    ButtonBackspace,
    ButtonEvaluate,
    ButtonChangeWidth,
    ButtonToggleSign,
};

class TouchButton {
  public:
    constexpr TouchButton(void)
        : area(0, 0, 0, 0), type(ButtonInvalid), disabled(true),
          selected(false) {}

    constexpr TouchButton(Area area, ButtonType type)
        : area(area), type(type), disabled(false), selected(false) {}

    /**
     * @brief Handle the touch input and execute corresponding commands if the
     * button is responsible for the input.
     * @param input The current touch input state
     * @return true if the button handled the input, false otherwise
     */
    bool
    ResponsibleFor(const Point &input) const {
        auto x = input.x;
        auto y = input.y;
        return (x >= area.x) && (x < area.x + area.w) && (y >= area.y) &&
               (y < area.y + area.h);
    }

    /**
     * @brief Disable the button, making it unresponsive to touch inputs.
     *
     */
    void
    Disable() {
        disabled = true;
        selected = false;
    }

    /**
     * @brief Enable the button, allowing it to respond to touch inputs.
     *
     */
    void
    Enable() {
        disabled = false;
    }

    /**
     * @brief Check if the button is currently disabled. A button is considered
     * disabled if it is not responsive to touch inputs.
     *
     * @return true if the button is disabled, false otherwise
     */
    bool
    Active() const {
        return !disabled;
    }

    /**
     * @brief Check if the button is currently selected. A button is considered
     * selected if it is currently being touched and is active (not disabled).
     *
     * @return true if the button is selected, false otherwise
     */
    bool
    Selected() const {
        return selected && Active();
    }

    void
    MarkSelected() {
        if (Active()) {
            selected = true;
        }
    }

    void
    Unselect() {
        selected = false;
    }

    ButtonType
    Type() const {
        return type;
    }

    static void ExecuteCommand(Commands &commands, ButtonType type);

  private:
    /**
     * @brief The area that this touch handler is responsible for.
     *
     */
    Area area;
    /**
     * @brief The type of the button, which determines which command to execute
     * when the button is touched.
     */
    ButtonType type;
    /**
     * @brief Whether the button is currently disabled. If true, the button will
     * not respond to touch inputs.
     */
    bool disabled;
    /**
     * @brief Whether the button is currently selected.
     */
    bool selected;
};

template <size_t N>
class TouchScreenHandler {
  public:
    TouchScreenHandler(Commands &commands)
        : commands(commands), buttons(), previouslySelected(nullptr), size(0) {}

    TouchButton *
    RegisterButton(const Area &area, ButtonType type) {
        assert(size < N);
        buttons[size++] = TouchButton(area, type);

        return &buttons[size - 1];
    }

    TouchButton &
    GetButton(size_t index) {
        assert(index < size);
        return buttons[index];
    }

    bool
    Handle(const Point &input) {
        for (size_t i = 0; i < N; i++) {
            TouchButton &button = buttons[i];
            if (button.Active() && button.ResponsibleFor(input)) {
                button.ExecuteCommand(commands, button.Type());
                if (previouslySelected) {
                    previouslySelected->Unselect();
                }
                button.MarkSelected();
                previouslySelected = &button;
                return true;
            }
        }
        return false;
    }

    size_t
    Size() const {
        return size;
    }

  private:
    Commands &commands;
    TouchButton buttons[N];
    TouchButton *previouslySelected;
    size_t size;
};

}; // namespace HexCalc
