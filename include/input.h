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

enum Direction {
    InvalidDir = 0,
    DirUp,
    DirDown,
    DirLeft,
    DirRight,
};

struct Point {
    int16_t x;
    int16_t y;

    constexpr Point(int16_t px, int16_t py) : x(px), y(py) {}

    constexpr Point(int data)
        : x(static_cast<int16_t>(data >> 16)),
          y(static_cast<int16_t>(data & 0xFFFF)) {}

    int
    ToInt(void) const {
        return (static_cast<int>(x) << 16) | static_cast<int>(y);
    }

    constexpr Point
    NextPosition(Direction dir) {
        auto x = this->x;
        auto y = this->y;

        switch (dir) {
        case Direction::DirUp:
            y -= 1;
            break;
        case Direction::DirDown:
            y += 1;
            break;
        case Direction::DirLeft:
            x -= 1;
            break;
        case Direction::DirRight:
            x += 1;
            break;
        default:
            // should never reach here
            break;
        }

        return Point(x, y);
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

template <int8_t W, int8_t H, uint8_t L>
struct CharArea {
    int8_t x;
    int8_t y;
    uint8_t w;
    uint8_t h;

    explicit CharArea(Area area)
        : x(area.x / W), y(area.y / H), w(area.w / W), h(area.h / (L * H)) {}

    static constexpr uint8_t lineHeight = L;
};

/**
 * @brief Area space for 6x8 font glyphs. Each unit corresponds to a 6x8 pixel
 * area on the screen, which is the size of a single glyph.
 *
 */
using Area6x8 = CharArea<6, 8, 2>;
using Area8x8 = CharArea<8, 8, 2>;

/**
 * @brief The InputHandler class is for reading inputs from the keys and the
 * touch screen.
 *
 */
class InputHandler {
  public:
    InputHandler(EventBus &eventBus, Commands &commands);

    void SetRepeat(int delay, int rate);

    void Update(void);

  private:
    EventBus &eventBus;
    Commands &commands;

    uint32_t heldKeys = 0;
    uint32_t currentKeys = 0;
    uint32_t previousKeys = 0;

    int repeatDelay = 20;
    int repeatRate = 4;

    bool stablePressed = false;
    bool previousTouch = false;

    Point smoothPos{0, 0};

    int pressCount = 0;
    int releaseCount = 0;

    static constexpr int PRESS_TH = 2;
    static constexpr int RELEASE_TH = 3;

    void updateKeys(void);
    void updateTouch(void);

    void handleKeyInput(void);
    void handleTouchInput(void);

    void notifyTouch(const Point &pos);
    void notifyPreviousTouch();
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
        : TouchButton(Area(0, 0, 0, 0), ButtonInvalid, 0, 0) {}

    constexpr TouchButton(Area area, ButtonType type, int16_t x, int16_t y)
        : area(area), type(type), position{x, y}, disabled(false),
          selected(false) {}

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
    Disable(void) {
        disabled = true;
        selected = false;
    }

    /**
     * @brief Enable the button, allowing it to respond to touch inputs.
     *
     */
    void
    Enable(void) {
        disabled = false;
    }

    /**
     * @brief Check if the button is currently disabled. A button is considered
     * disabled if it is not responsive to touch inputs.
     *
     * @return true if the button is disabled, false otherwise
     */
    bool
    Active(void) const {
        return !disabled;
    }

    /**
     * @brief Check if the button is currently selected. A button is considered
     * selected if it is currently being touched and is active (not disabled).
     *
     * @return true if the button is selected, false otherwise
     */
    bool
    Selected(void) const {
        return selected && Active();
    }

    void
    MarkSelected(void) {
        if (Active()) {
            selected = true;
        }
    }

    void
    Unselect(void) {
        selected = false;
    }

    ButtonType
    Type(void) const {
        return type;
    }

    Point
    Position(void) const {
        return position;
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

    Point position;

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

template <size_t M, size_t N>
class TouchScreenHandler {
  public:
    TouchScreenHandler(Commands &commands)
        : commands(commands), buttons(), buttonMatrix{{nullptr}},
          previouslySelected(nullptr), size(0) {}

    TouchButton *
    RegisterButton(const Area &area, ButtonType type, int16_t m, int16_t n) {
        assert(m >= 0 && m < M);
        assert(n >= 0 && n < N);
        assert(size < Capacity());

        auto &button = buttons[size++];
        button = TouchButton(area, type, m, n);

        buttonMatrix[m][n] = &button;

        return &button;
    }

    TouchButton &
    GetButton(size_t index) {
        assert(index < size);
        return buttons[index];
    }

    TouchButton *
    GetMatrix(Point position) {
        auto m = (position.x + M) % M;
        auto n = (position.y + N) % N;

        return buttonMatrix[m][n];
    }

    bool
    Handle(const Point &input) {
        for (size_t i = 0; i < Capacity(); i++) {
            auto &button = buttons[i];
            if (button.Active() && button.ResponsibleFor(input)) {
                button.ExecuteCommand(commands, button.Type());
                ChangeFocus(&button);
                return true;
            }
        }

        ChangeFocus(nullptr);
        return false;
    }

    /**
     * @brief Get the currently focused button. Use ChangeFocus() to change the
     * focused button.
     *
     * @return const TouchButton* currently focused button, or nullptr if no
     * button is currently selected
     */
    const TouchButton *
    FocusedButton() const {
        return previouslySelected;
    }

    /**
     * @brief Change the currently focused button to the given button.
     *
     * @param button The button to focus, or nullptr to clear the focus.
     */
    void
    ChangeFocus(TouchButton *button) {
        if (previouslySelected != nullptr) {
            previouslySelected->Unselect();
        }
        if (button != nullptr) {
            button->MarkSelected();
        }
        previouslySelected = button;
    }

    Point
    NavigateFocus(Point position, Direction dir) {
        Point nextPos = position.NextPosition(dir);

        auto *button = GetMatrix(nextPos);

        // this must be under '+'
        if (button == nullptr) {
            nextPos = nextPos.NextPosition(DirUp);
        }
        // skip width and sign drawers
        if (((nextPos.x == 3) || (nextPos.x == 4)) && (nextPos.y == 0)) {
            nextPos = nextPos.NextPosition(DirDown);
        }
        // skip evaluate button
        if (nextPos.x == 4 && nextPos.y == 6) {
            nextPos = nextPos.NextPosition(dir);
        }

        return nextPos;
    }

    void
    PressFocus(void) {
        if (previouslySelected) {
            auto &button = *previouslySelected;
            button.ExecuteCommand(commands, button.Type());
        }
    }

    size_t
    Size(void) const {
        return size;
    }

    static constexpr size_t Width = M;
    static constexpr size_t Height = N;

    static constexpr size_t
    Capacity(void) {
        return (M * N);
    }

  private:
    Commands &commands;
    TouchButton buttons[Capacity()];
    TouchButton *buttonMatrix[M][N];
    TouchButton *previouslySelected;
    size_t size;
};
}; // namespace HexCalc
