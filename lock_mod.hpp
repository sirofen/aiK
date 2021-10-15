#pragma once
#include <cstdint>

namespace lock_mod {
enum class lock_status : std::uint8_t {
    LOCK,
    UNLOCK
};
enum class lock_ents : std::uint8_t {
    LOAD_DRIVER,
    CONSOLE_MOD,
    PLAYER_MOD
};
}
