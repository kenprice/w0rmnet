#include <stdlib.h>
#include "programs.h"

const char* ProgramTypeLabel[] = {
    "Ping",
    "Echo",
    "Network Scanner",
    "Remote Login",
    "Packet Filter"
};

const char* ProgramNames[][5] = {
    // PING
    { "Ping", "Pong", "Pinger", "Ponger", "PingPong" },
    // ECHO
    { "Echo", "Remote Echo", "Parrot", NULL },
    // SCAN
    { "Netscan", "Masspinger", "Netmapper", NULL },
    // LOGIN
    { "RemoteShell", "RemoteLogin", "RTerminal", "ShTTy", "TermNet" },
    // CHECK_ORIGIN
    { "MyFirewall", "XtraSecure Firewall", "XProtecc Gate", "FlameBarrier", "GreatWall" }
};

Program PingPrograms[] = {
    {ProgramTypePing, "NetRipple Ping", 1, 0 },
    {ProgramTypePing, "NetRipple Ping", 1, 2 },
    {ProgramTypePing, "QuantumPing",    1, 0 },
    {ProgramTypePing, "QuantumPing",    1, 2 },
    {ProgramTypePing, "Bingo Ping",     1, 1 },
    {ProgramTypePing, "Bingo Ping",     1, 2 },
};

Program EchoPrograms[] = {
    {ProgramTypeEcho, "NetRipple Echo", 1, 0 },
    {ProgramTypeEcho, "NetRipple Echo", 1, 2 },
    {ProgramTypeEcho, "QuantumEcho",    1, 0 },
    {ProgramTypeEcho, "QuantumEcho",    1, 2 },
    {ProgramTypeEcho, "Bingo Echo",     1, 1 },
    {ProgramTypeEcho, "Bingo Echo",     1, 2 },
};

Program ScanPrograms[] = {
    {ProgramTypeScan, "NetRipple Scan",     1, 0 },
    {ProgramTypeScan, "NetRipple Scan",     1, 3 },
    {ProgramTypeScan, "Quantum NetScanner", 1, 3 },
    {ProgramTypeScan, "Quantum NetScanner", 2, 1 },
    {ProgramTypeScan, "Bingo Network Scan", 2, 1 },
    {ProgramTypeScan, "Bingo Network Scan", 2, 3 },
};

Program LoginPrograms[] = {
    {ProgramTypeLogin, "NetRipple Login",     1, 0 },
    {ProgramTypeLogin, "NetRipple Login",     2, 2 },
    {ProgramTypeLogin, "Quantum RemoteShell", 1, 1 },
    {ProgramTypeLogin, "Quantum RemoteShell", 2, 1 },
    {ProgramTypeLogin, "Bingo Remote Login",  1, 0 },
    {ProgramTypeLogin, "Bingo Remote Login",  1, 2 },
};

Program CheckOriginPrograms[] = {
    { PROCESS_TYPE_CHECK_ORIGIN, "CyberBarrier FireGuard", 3,  0 },
    { PROCESS_TYPE_CHECK_ORIGIN, "CyberBarrier FireGuard", 4,  1 },
    { PROCESS_TYPE_CHECK_ORIGIN, "DataFortress Firewall", 1,  1 },
    { PROCESS_TYPE_CHECK_ORIGIN, "DataFortress Firewall", 1,  2 },
    { PROCESS_TYPE_CHECK_ORIGIN, "DigiShield InfernoWall", 1,  2 },
    { PROCESS_TYPE_CHECK_ORIGIN, "DigiShield InfernoWall", 3,  2 },
};