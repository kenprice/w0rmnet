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
    {ProgramTypePing, "NetRipple Ping", 100 },
    {ProgramTypePing, "NetRipple Ping", 102 },
    {ProgramTypePing, "QuantumPing",    100 },
    {ProgramTypePing, "QuantumPing",    102 },
    {ProgramTypePing, "Bingo Ping",     101 },
    {ProgramTypePing, "Bingo Ping",     102 },
};

Program EchoPrograms[] = {
    {ProgramTypeEcho, "NetRipple Echo", 100 },
    {ProgramTypeEcho, "NetRipple Echo", 102 },
    {ProgramTypeEcho, "QuantumEcho",    100 },
    {ProgramTypeEcho, "QuantumEcho",    102 },
    {ProgramTypeEcho, "Bingo Echo",     101 },
    {ProgramTypeEcho, "Bingo Echo",     102 },
};

Program ScanPrograms[] = {
    {ProgramTypeScan, "NetRipple Scan",     100 },
    {ProgramTypeScan, "NetRipple Scan",     103 },
    {ProgramTypeScan, "Quantum NetScanner", 103 },
    {ProgramTypeScan, "Quantum NetScanner", 201 },
    {ProgramTypeScan, "Bingo Network Scan", 201 },
    {ProgramTypeScan, "Bingo Network Scan", 203 },
};

Program LoginPrograms[] = {
    {ProgramTypeLogin, "NetRipple Login",     100 },
    {ProgramTypeLogin, "NetRipple Login",     202 },
    {ProgramTypeLogin, "Quantum RemoteShell", 101 },
    {ProgramTypeLogin, "Quantum RemoteShell", 201 },
    {ProgramTypeLogin, "Bingo Remote Login",  100 },
    {ProgramTypeLogin, "Bingo Remote Login",  102 },
};

Program CheckOriginPrograms[] = {
    { PROCESS_TYPE_CHECK_ORIGIN, "CyberBarrier FireGuard", 300 },
    { PROCESS_TYPE_CHECK_ORIGIN, "CyberBarrier FireGuard", 401 },
    { PROCESS_TYPE_CHECK_ORIGIN, "DataFortress Firewall",  101 },
    { PROCESS_TYPE_CHECK_ORIGIN, "DataFortress Firewall",  102 },
    { PROCESS_TYPE_CHECK_ORIGIN, "DigiShield InfernoWall", 102 },
    { PROCESS_TYPE_CHECK_ORIGIN, "DigiShield InfernoWall", 302 },
};