#pragma once

#include "Common/Singleton/Singleton.h"

#include "Core/Math/Math.h"

#include <map>
#include <string>
#include <memory>

namespace Mrk
{
    enum class MouseBtn
    {
        Left = 0,
        Right = 1,
        Middle = 2,
    };

    // same with ImGui
    enum class Key : int
    {
        None = 0,
        Tab = 512,
        LeftArrow,
        RightArrow,
        UpArrow,
        DownArrow,
        PageUp,
        PageDown,
        Home,
        End,
        Insert,
        Delete,
        Backspace,
        Space,
        Enter,
        Escape,
        LeftCtrl, LeftShift, LeftAlt, LeftSuper,
        RightCtrl, RightShift, RightAlt, RightSuper,
        Menu,
        Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
        A, B, C, D, E, F, G, H, I, J,
        K, L, M, N, O, P, Q, R, S, T,
        U, V, W, X, Y, Z,
        F1, F2, F3, F4, F5, F6,
        F7, F8, F9, F10, F11, F12,
        F13, F14, F15, F16, F17, F18,
        F19, F20, F21, F22, F23, F24,
        Apostrophe,        // '
        Comma,             // ,
        Minus,             // -
        Period,            // .
        Slash,             // /
        Semicolon,         // ;
        Equal,             // =
        LeftBracket,       // [
        Backslash,         // \ (this text inhibit multiline comment caused by backslash)
        RightBracket,      // ]
        GraveAccent,       // `
        CapsLock,
        ScrollLock,
        NumLock,
        PrintScreen,
        Pause,
        pad0, pad1, pad2, pad3, pad4,
        pad5, pad6, pad7, pad8, pad9,
        padDecimal,
        padDivide,
        padMultiply,
        padSubtract,
        padAdd,
        padEnter,
        padEqual,

        ImGuiMod_Ctrl = 1 << 12,
        ImGuiMod_Shift = 1 << 13,
        ImGuiMod_Alt = 1 << 14,
    };

    struct InputIO
    {
        friend class InputSys;
    public:
        InputIO() = default;

        void SetRect(int left, int top, int right, int bottom);
        void SetLayer(int layer);
        void SetEnable(bool enable);

        bool IsKeyDown(Key key);
        bool IsKeyPress(Key key);
        bool IsKeyRelease(Key key);

        bool IsMouseDown(MouseBtn btn);
        bool IsMouseClick(MouseBtn btn);
        bool IsMouseDoubleClick(MouseBtn btn);
        bool IsMouseRelease(MouseBtn btn);

        float GetMouseWheel();
        Vector2 GetMousePos();
        Vector2 GetMouseDelta();

    private:
        int left = -1;
        int top = -1;
        int right = -1;
        int bottom = -1;

        int layer = 0;
        bool enable = false;
        bool isMouseIn = false;
    };

    class InputSys : public Singleton<InputSys>
    {
        MRK_SINGLETON(InputSys)
    private:
        InputSys() = default;
    public:
        static void Update();

        static InputIO& GetInputIO(std::string_view name);

        static bool IsKeyDown(InputIO& io, Key key);
        static bool IsKeyPress(InputIO& io, Key key);
        static bool IsKeyRelease(InputIO& io, Key key);

        static bool IsMouseDown(InputIO& io, MouseBtn btn);
        static bool IsMouseClick(InputIO& io, MouseBtn btn);
        static bool IsMouseDoubleClick(InputIO& io, MouseBtn btn);
        static bool IsMouseRelease(InputIO& io, MouseBtn btn);

        static float GetMouseWheel(InputIO& io);
        static Vector2 GetMousePos(InputIO& io);
        static Vector2 GetMouseDelta(InputIO& io);

    private:
        std::map<std::string, InputIO> ios;
    };
}