#pragma once
#include <functional>
#include <array>

enum class Event : uint8_t
{
    ClockTick,
    Start,
    Stop,
    Continue,
    NoteOn,
    NoteOff,
};

class EventBus
{
public:
    using Handler = std::function<void()>;
    static void subscribe(Event e, Handler h)
    {
        auto &slot = _slots[static_cast<uint8_t>(e)];
        slot = h;
    }
    static void publish(Event e)
    {
        auto &slot = _slots[static_cast<uint8_t>(e)];
        if (slot)
            slot();
    }

private:
    static inline std::array<Handler, 6> _slots{};
};
