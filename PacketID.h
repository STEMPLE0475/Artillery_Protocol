#pragma once

// 21 부터 256
enum class PACKET_ID : short
{
    // 1) Auth
    LOGIN_REQ = 21, LOGIN_RES = 22,
    REGISTER_REQ = 23, REGISTER_RES = 24,

    // 2) Lobby/Match
    MATCH_REQUEST_REQ = 31, MATCH_REQUEST_RES = 32,
    MATCH_CANCEL_REQ = 33, MATCH_CANCEL_RES = 34,
    MATCH_COMPLETE_NTF = 35,
    TEAM_ASSIGN_NTF = 36,

    // 3) Game Core
    GAME_START_NTF = 41,
    GAME_END_NTF = 42,

    // Movement
    PLAYER_MOVE_REQ = 51, PLAYER_MOVE_NTF = 52,
    PLAYER_STOP_REQ = 53, PLAYER_STOP_NTF = 54,
    STATE_SNAPSHOT_NTF = 55,

    // Firing / Projectile / Terrain
    FIRE_READY_REQ = 61, FIRE_READY_RES = 62,
    FIRE_START_REQ = 63, FIRE_START_RES = 64,
    PROJECTILE_CREATE_NTF = 65,
    PROJECTILE_EXPLODE_NTF = 66,
    TERRAIN_UPDATE_NTF = 67,

    // Character state
    PLAYER_HIT_REQ = 71,
    PLAYER_DEAD_NTF = 72,
    PLAYER_RESPAWN_NTF = 73,
    SCORE_UPDATE_NTF = 74,

    // Chat
    CHAT_REQ = 81, CHAT_RES = 82, CHAT_NTF = 83,
    ECHO_CHAT_REQ = 84, ECHO_CHAT_NTY = 85,

    // Misc
    PING_REQ = 101, PING_RES = 102,
    PLAYER_DISCONNECTED_NTF = 103,
    ERROR_NTF = 104,
    TICK_RATE_NTF = 105,


};

