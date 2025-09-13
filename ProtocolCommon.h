#pragma once

#include "PacketID.h"
#include "ErrorCode.h"
#include <cstdint>

#pragma pack(push, 1)

struct PktHeader
{
    int16_t TotalSize;
    int16_t Id;                 // PACKET_ID
    uint8_t Reserve;            // 예비(암호화/압축/버전 플래그 등)
};



// 클라 전용 상수
constexpr uint32_t PACKET_HEADER_SIZE = sizeof(PktHeader);
constexpr uint32_t CLIENT_MAX_PACKET_SIZE = 8 * 1024;

struct PktBase
{
    int16_t ErrorCode = (int16_t)ERROR_CODE::NONE;
    void SetError(ERROR_CODE error) { ErrorCode = (int16_t)error; }
};

// ───────────────────────────────────────────────────────────────
// 공통 상수
// ───────────────────────────────────────────────────────────────
const int32_t MAX_USER_ID_SIZE = 16;   // ASCII only
const int32_t MAX_USER_PASSWORD_SIZE = 16;   // ASCII only
const int32_t MAX_USER_NICKNAME_SIZE = 16;   // UTF-8 bytes (payload max)
const int32_t MAX_ROOM_TITLE_SIZE = 16;
const int32_t MAX_ROOM_CHAT_MSG_SIZE = 256;  // UTF-8 bytes (payload max)
const int32_t MAX_PLAYERS_PER_ROOM = 4;  
const int32_t MAX_PROJECTILES_SNAPSHOT = 32;   // snapshot당 최대 투사체 반영 수
const int32_t MAX_CHAT_LEN = 200;  // 에코 테스트용
const int32_t MAX_ID_LEN = 32;
const int32_t MAX_PW_LEN = 32;
const int32_t MAX_NICKNAME_LEN = 32;

// ───────────────────────────────────────────────────────────────
// 1) 인증 / 계정 (21~24)
// ───────────────────────────────────────────────────────────────
// 운영 편의: ID/PW는 ASCII 고정(NUL 패딩), Nick은 UTF-8 가변
struct PktLogInReq
{
    uint8_t szID[MAX_USER_ID_SIZE] = { 0, }; // ASCII
    uint8_t szPW[MAX_USER_PASSWORD_SIZE] = { 0, }; // ASCII
};

struct PktLogInRes : PktBase
{
    uint16_t NickLen = 0;                                 // UTF-8 bytes
    uint8_t  Nick[MAX_USER_NICKNAME_SIZE] = { 0, };       // UTF-8
};

struct PktRegisterReq
{
    uint8_t szID[MAX_USER_ID_SIZE] = { 0, };        // ASCII
    uint8_t szPW[MAX_USER_PASSWORD_SIZE] = { 0, };        // ASCII
    uint16_t NickLen = 0;                                 // UTF-8 bytes
    uint8_t  Nick[MAX_USER_NICKNAME_SIZE] = { 0, };       // UTF-8
};

struct PktRegisterRes : PktBase
{
};


// ───────────────────────────────────────────────────────────────
// 2) 로비 / 매칭 (31~36)
// ───────────────────────────────────────────────────────────────
struct PktMatchRequestReq
{
    //uint8_t PreferredTeam; // 0: 아무거나, 1:A, 2:B
};

struct PktMatchRequestRes : PktBase
{
    //uint32_t EstimatedMs; // 대기 예측(옵션)
};

struct PktMatchCancelReq
{
};

struct PktMatchCancelRes : PktBase
{
};

struct PlayerSpawnInfo {
    int32_t UserHandle;
    uint8_t Team;
    uint8_t Slot;
    float SpawnX;
    float SpawnY;
};

struct PktMatchCompleteNtf
{
    uint8_t         PlayerCount; 
    uint8_t         MyPlayerIndexInList;
    PlayerSpawnInfo PlayerList[MAX_PLAYERS_PER_ROOM];
};

struct PktTeamAssignNtf
{
    uint8_t Team;
    uint8_t Slot;
};

// ───────────────────────────────────────────────────────────────
// 3) 게임 진행 (41~74)
// ───────────────────────────────────────────────────────────────

struct PktGameStartNtf
{
    uint32_t StartServerTimeMs; // 카운트다운 동기화용
    uint16_t CountdownMs;       // 0이면 즉시
};


// [플레이어 입력/이동] 51~55
// 입력 비트마스크: bit0=Left, bit1=Right, bit2=Jump, bit3=Skill1, bit4=Skill2 ...
struct PktPlayerMoveReq
{
    uint8_t  InputBits;
    uint16_t ClientSeq;      // 클라 입력 시퀀스
    uint32_t ClientTimeMs;   // RTT/보정용
};

// 알림/스냅샷 등 빈번 전송은 문자열 대신 핸들 사용(대역폭 절약)
struct PktPlayerMoveNtf
{
    uint32_t UserHandle; // 내부 숫자 핸들
    float    X;
    float    Y;
    float    VX;
    float    VY;
    uint8_t  Facing;     // 0:left,1:right
    uint16_t ServerTick;
};

struct PktPlayerStopReq
{
    uint16_t ClientSeq;
};

struct PktPlayerStopNtf
{
    uint32_t UserHandle;
    float    X;
    float    Y;
    uint16_t ServerTick;
};

// 상태 스냅샷(주기 송신)
struct SnapshotPlayerState
{
    uint32_t UserHandle; // 숫자 핸들
    float    X;
    float    Y;
    float    VX;
    float    VY;
    int16_t  HP;
    uint8_t  OnGround;   // bool
    float    AimAngle;   // 라디안 or 도(약속)
};

struct PktStateSnapshotNtf
{
    uint16_t              ServerTick;
    uint8_t               PlayerCount;
    SnapshotPlayerState   Players[MAX_PLAYERS_PER_ROOM];
};

// [공격/발사 플로우] 61~67
struct PktFireReadyReq
{
    float    AimAngle;   // 입력 각
    float    PosX;       // 서버 검증용
    float    PosY;
    uint8_t  WeaponType; // 0:기본,1:특수 ...
};

struct PktFireReadyRes : PktBase
{
    float AimAngleClamped; // 서버 정규화/클램프 결과
};

struct PktFireStartReq
{
    float    AimAngle;   // 최종 각
    float    ChargeSec;  // 홀드 시간 -> 파워
    uint8_t  WeaponType;
    uint16_t ClientSeq;
};

struct PktFireStartRes : PktBase
{
    // OK 시, 이어서 PROJECTILE_CREATE_NTF 송신
};

struct PktProjectileCreateNtf
{
    uint32_t ProjectileId;
    uint32_t OwnerHandle; // 소유자 핸들
    float    X;
    float    Y;
    float    VX;
    float    VY;
    float    Gravity;
    uint8_t  WeaponType;
    uint16_t ServerTick;
};

struct PktProjectileExplodeNtf
{
    uint32_t ProjectileId;
    float    X;
    float    Y;
    float    Radius;
    uint16_t BaseDamage;
    uint8_t  StatusFlags; // 화상/넉백 등
    uint16_t ServerTick;
};

// 지형 파괴: 원형 디폼 1건(간단형)
struct PktTerrainUpdateNtf
{
    uint32_t PatchId;          // 클라/서버 동기 체크
    float    CenterX;
    float    CenterY;
    float    Radius;
    float    Depth;            // 양수=새김(깎임) 정도
    uint32_t TerrainHashAfter; // 적용 후 지형 해시
};

// [캐릭터 상태] 71~74
struct PktPlayerHitNtf
{
    uint32_t VictimHandle;
    uint32_t AttackerHandle;
    uint16_t Damage;
    int16_t  RemainHP;
    uint8_t  HitType; // 직격/스플래시 등
};

struct PktGameEndNtf
{
    uint8_t  WinningTeam; // 0: A팀 승리, 1: B팀 승리, 255: 무승부
    uint16_t TeamAScore;
    uint16_t TeamBScore;
};

struct PktPlayerDeadNtf
{
    uint32_t VictimHandle;
    uint32_t AttackerHandle;
    uint32_t RespawnMs; // 0이면 리스폰 없음(라운드 종료형)
};

struct PktPlayerRespawnNtf
{
    uint32_t UserHandle;
    float    X;
    float    Y;
};

struct PktScoreUpdateNtf
{
    uint16_t TeamAScore;
    uint16_t TeamBScore;
};

// ───────────────────────────────────────────────────────────────
// 4) 채팅 (81~83) — UTF-8 + 길이(바이트)
// ───────────────────────────────────────────────────────────────
struct PktChatReq
{
    uint8_t  Channel;                          // 0:전체,1:팀,2:파티...
    uint16_t MsgLen;                           // UTF-8 byte length
    uint8_t  Msg[MAX_ROOM_CHAT_MSG_SIZE];      // UTF-8 bytes (no NUL)
};

struct PktChatRes : PktBase
{
};

struct PktChatNtf
{
    uint8_t  Channel;
    uint32_t SenderHandle;                     // 발신자 핸들
    uint16_t MsgLen;                           // UTF-8 bytes
    uint8_t  Msg[MAX_ROOM_CHAT_MSG_SIZE];      // UTF-8
};

// 에코 테스트용
struct PktEchoChatReq
{
    uint16_t Len = 0;                          // UTF-8 bytes
    uint8_t  Msg[MAX_CHAT_LEN] = { 0, };       // UTF-8
};

struct PktEchoChatNty
{
    uint16_t Len = 0;                          // UTF-8 bytes
    uint8_t  Msg[MAX_CHAT_LEN] = { 0, };       // UTF-8
};

// ───────────────────────────────────────────────────────────────
// 5) 기타 / 상태 (101~105)
// ───────────────────────────────────────────────────────────────
struct PktPingReq
{
    uint32_t ClientTimeMs;
};

struct PktPingRes
{
    uint32_t ClientTimeMs;
    uint32_t ServerTimeMs;
};

struct PktPlayerDisconnectedNtf
{
    uint32_t UserHandle;
};

struct PktErrorNtf
{
    int16_t RefPacketId; // 어떤 패킷 처리 중 에러인지
    int16_t ErrorCode;   // ERROR_CODE
};

struct PktTickRateNtf
{
    uint16_t ServerHz;    // 서버 틱레이트
    uint16_t SnapshotHz;  // 스냅샷 주기(Hz)
};

#pragma pack(pop)