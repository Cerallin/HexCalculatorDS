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

    explicit constexpr Point(int data)
        : x(static_cast<int16_t>(data >> 16)),
          y(static_cast<int16_t>(data & 0xFFFF)) {}

    int
    ToInt(void) const {
        return (static_cast<int>(x) << 16) | static_cast<int>(y);
    }

    template <int16_t colNum, int16_t rowNum>
    HEXCALC_ARM_CODE constexpr Point
    NextPosition(Direction dir) const {
        auto nextX = this->x;
        auto nextY = this->y;

        switch (dir) {
        case Direction::DirUp:
            nextY = (nextY - 1 + rowNum) % rowNum;
            break;
        case Direction::DirDown:
            nextY = (nextY + 1) % rowNum;
            break;
        case Direction::DirLeft:
            nextX = (nextX - 1 + colNum) % colNum;
            break;
        case Direction::DirRight:
            nextX = (nextX + 1) % colNum;
            break;
        default:
            // should never reach here
            break;
        }

        return Point(nextX, nextY);
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

        x = x_min;
        y = y_min;
        w = static_cast<uint8_t>(x_max - x_min);
        h = static_cast<uint8_t>(y_max - y_min);
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

    explicit CharArea(const Area &area)
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

    EventResult HandleEvent(const Event &e);

    enum class KeyAction : uint8_t {
        PressDown = 1,
        PressUp = 2,
    };

    enum class TouchAction : uint8_t {
        TouchDown = 1,
        TouchUp = 2,
    };

  private:
    struct KeyRepeatState {
        int counter = 0;
    };

    EventBus &eventBus;
    Commands &commands;

    uint32_t heldKeys = 0;
    uint32_t previousHeldKeys = 0;

    int repeatDelay = 20;
    int repeatRate = 4;

    bool stablePressed = false;
    bool previousTouch = false;

    Point smoothPos{0, 0};

    int pressCount = 0;
    int releaseCount = 0;

    static constexpr int PRESS_TH = 2;
    static constexpr int RELEASE_TH = 3;

    static constexpr int KEY_COUNT = 32;
    KeyRepeatState keyStates[KEY_COUNT];

    void updateKeys(uint32_t newHeldKeys);
    void updateTouch(bool rawPressed, const Point &rawPoint);

    void postKeyEvent(KeyAction action, uint32_t keyMask);
    void postTouchEvent(TouchAction action, const Point &pos);

    void dispatchKeyPressDown(uint32_t keyMask);
};

enum ButtonType : uint8_t {
    ButtonInvalid = 0xFF,
    // Input buttons
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
    // Drawer buttons
    ButtonChangeWidth,
    ButtonToggleSign,
    // Editor buttons
    ButtonToggleView,
    ButtonLShiftMode,
    ButtonRShiftMode,
    // Shift directly instead of adding operators
    ButtonLShiftBin,
    ButtonRShiftBin,
    // Digit pad buttons
    ButtonFlipBit,
};

class TouchButton {
  public:
    constexpr TouchButton(void)
        : TouchButton(Area(0, 0, 0, 0), ButtonInvalid, 0, 0, 0) {}

    constexpr TouchButton(Area area, ButtonType type, int16_t x, int16_t y,
                          int index = 0)
        : area(area), type(type), position{x, y}, index(index), disabled(false),
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

    int
    Index(void) const {
        return index;
    }

    static void ExecuteCommand(Commands &commands, ButtonType type,
                               int index = 0);

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

    int index;

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
    explicit TouchScreenHandler(Commands &commands)
        : commands(commands), buttons(), buttonMatrix{{nullptr}},
          previouslySelected(nullptr), size(0) {}

    TouchButton *
    RegisterButton(const Area &area, ButtonType type, int16_t m, int16_t n,
                   int index = 0) {
        constexpr int16_t width = static_cast<int16_t>(M);
        constexpr int16_t height = static_cast<int16_t>(N);

        assert(m >= 0 && m < width);
        assert(n >= 0 && n < height);
        assert(size < Capacity());

        auto &button = buttons[size++];
        button = TouchButton(area, type, m, n, index);

        buttonMatrix[m][n] = &button;

        return &button;
    }

    TouchButton &
    GetButton(size_t index) {
        assert(index < size);
        return buttons[index];
    }

    const TouchButton &
    GetButton(size_t index) const {
        assert(index < size);
        return buttons[index];
    }

    HEXCALC_ARM_CODE
    TouchButton *
    GetMatrix(Point position) {
        auto m = (position.x + M) % M;
        auto n = (position.y + N) % N;

        return buttonMatrix[m][n];
    }

    HEXCALC_ARM_CODE
    const TouchButton *
    GetMatrix(Point position) const {
        auto m = (position.x + M) % M;
        auto n = (position.y + N) % N;

        return buttonMatrix[m][n];
    }

    bool
    Handle(const Point &input) {
        for (size_t i = 0; i < Capacity(); i++) {
            auto &button = buttons[i];
            if (button.Active() && button.ResponsibleFor(input)) {
                button.ExecuteCommand(commands, button.Type(), button.Index());
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
     * @brief Registration index of the focused button, or -1 if none.
     */
    int
    FocusedIndex(void) const {
        if (previouslySelected == nullptr) {
            return -1;
        }

        for (size_t i = 0; i < size; i++) {
            if (&buttons[i] == previouslySelected) {
                return static_cast<int>(i);
            }
        }

        return -1;
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

    /**
     * @brief Clear focus when the focused button is no longer active.
     */
    void
    ClearFocusIfInactive(void) {
        if ((previouslySelected != nullptr) && !previouslySelected->Active()) {
            ChangeFocus(nullptr);
        }
    }

    void
    PressFocus(void) {
        if (previouslySelected) {
            const auto &button = *previouslySelected;
            button.ExecuteCommand(commands, button.Type(), button.Index());
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

/**
 * @brief Touch handler for the InputView keyboard (5x7), including D-pad
 *        navigation that skips drawers / evaluate and inactive keys.
 */
class InputTouchScreenHandler : public TouchScreenHandler<5, 7> {
  public:
    using TouchScreenHandler::TouchScreenHandler;

    static constexpr int16_t ColNum = 5;
    static constexpr int16_t RowNum = 7;

    Point
    NavigateFocus(Point position, Direction dir) {
        Point nextPos = position.NextPosition<ColNum, RowNum>(dir);

        auto *button = GetMatrix(nextPos);

        // Empty cell under '+': step up onto the plus key.
        if (button == nullptr) {
            nextPos = nextPos.NextPosition<ColNum, RowNum>(DirUp);
        }
        // Skip width and sign drawers.
        if (((nextPos.x == 3) || (nextPos.x == 4)) && (nextPos.y == 0)) {
            nextPos = nextPos.NextPosition<ColNum, RowNum>(DirDown);
        }
        // Skip evaluate button.
        if (nextPos.x == 4 && nextPos.y == 6) {
            nextPos = nextPos.NextPosition<ColNum, RowNum>(dir);
        }

        return nextPos;
    }

    TouchButton &
    FindFocus(Point position, Direction dir) {
        Point nextPos = position;
        for (size_t step = 0; step < Capacity(); step++) {
            nextPos = NavigateFocus(nextPos, dir);

            auto *buttonPtr = GetMatrix(nextPos);
            assert(buttonPtr != nullptr);

            if (buttonPtr->Active()) {
                return *buttonPtr;
            }
        }

        // Fallback: should not happen while any keyboard key remains active.
        auto *fallback = GetMatrix(nextPos);
        assert(fallback != nullptr);
        return *fallback;
    }

    void
    MoveFocus(Direction dir) {
        Point pos{0, 0};
        const auto *focused = FocusedButton();
        if (focused != nullptr) {
            pos = focused->Position();
        }
        ChangeFocus(&FindFocus(pos, dir));
    }
};

/**
 * @brief Touch handler for EditorView action buttons (5x3). Thin wrapper for
 *        now; D-pad navigation can be added here later.
 */
class EditorTouchScreenHandler : public TouchScreenHandler<5, 3> {
  public:
    using TouchScreenHandler::TouchScreenHandler;
};
}; // namespace HexCalc
