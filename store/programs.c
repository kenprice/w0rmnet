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
    {ProgramTypeScan, "QuantumScanner", 103 },
    {ProgramTypeScan, "QuantumScanner", 201 },
    {ProgramTypeScan, "Netscan", 201 },
    {ProgramTypeScan, "Netscan", 203 },
};

Program LoginPrograms[] = {
    {ProgramTypeLogin, "NetRipple Login",     100 },
    {ProgramTypeLogin, "NetRipple Login",     202 },
    {ProgramTypeLogin, "QuantumShell", 101 },
    {ProgramTypeLogin, "QuantumShell", 201 },
    {ProgramTypeLogin, "ShTTy",  100 },
    {ProgramTypeLogin, "ShTTy",  102 },
};

Program CheckOriginPrograms[] = {
    { PROCESS_TYPE_CHECK_ORIGIN, "CyberBarrier", 300 },
    { PROCESS_TYPE_CHECK_ORIGIN, "CyberBarrier", 401 },
    { PROCESS_TYPE_CHECK_ORIGIN, "DataFortress",  101 },
    { PROCESS_TYPE_CHECK_ORIGIN, "DataFortress",  102 },
    { PROCESS_TYPE_CHECK_ORIGIN, "DigiShield", 102 },
    { PROCESS_TYPE_CHECK_ORIGIN, "DigiShield", 302 },
};