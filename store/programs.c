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
    { PROCESS_TYPE_PING, "NetRipple Ping", 1,  0 },
    { PROCESS_TYPE_PING, "NetRipple Ping", 1,  2 },
    { PROCESS_TYPE_PING, "QuantumPing", 1,  0 },
    { PROCESS_TYPE_PING, "QuantumPing", 1,  2 },
    { PROCESS_TYPE_PING, "Bingo Ping", 1,  1 },
    { PROCESS_TYPE_PING, "Bingo Ping", 1,  2 },
};

Program EchoPrograms[] = {
    { PROCESS_TYPE_ECHO, "NetRipple Echo", 1,  0 },
    { PROCESS_TYPE_ECHO, "NetRipple Echo", 1,  2 },
    { PROCESS_TYPE_ECHO, "QuantumEcho", 1,  0 },
    { PROCESS_TYPE_ECHO, "QuantumEcho", 1,  2 },
    { PROCESS_TYPE_ECHO, "Bingo Echo", 1,  1 },
    { PROCESS_TYPE_ECHO, "Bingo Echo", 1,  2 },
};

Program ScanPrograms[] = {
    { PROCESS_TYPE_SCAN, "NetRipple Scan", 1,  0 },
    { PROCESS_TYPE_SCAN, "NetRipple Scan", 1,  3 },
    { PROCESS_TYPE_SCAN, "Quantum NetScanner", 1,  3 },
    { PROCESS_TYPE_SCAN, "Quantum NetScanner", 2,  1 },
    { PROCESS_TYPE_SCAN, "Bingo Network Scan", 2,  1 },
    { PROCESS_TYPE_SCAN, "Bingo Network Scan", 2,  3 },
};

Program LoginPrograms[] = {
    { PROCESS_TYPE_LOGIN, "NetRipple Login", 1,  0 },
    { PROCESS_TYPE_LOGIN, "NetRipple Login", 2,  2 },
    { PROCESS_TYPE_LOGIN, "Quantum RemoteShell", 1,  1 },
    { PROCESS_TYPE_LOGIN, "Quantum RemoteShell", 2,  1 },
    { PROCESS_TYPE_LOGIN, "Bingo Remote Login", 1,  0 },
    { PROCESS_TYPE_LOGIN, "Bingo Remote Login", 1,  2 },
};

Program CheckOriginPrograms[] = {
    { PROCESS_TYPE_CHECK_ORIGIN, "CyberBarrier FireGuard", 3,  0 },
    { PROCESS_TYPE_CHECK_ORIGIN, "CyberBarrier FireGuard", 4,  1 },
    { PROCESS_TYPE_CHECK_ORIGIN, "DataFortress Firewall", 1,  1 },
    { PROCESS_TYPE_CHECK_ORIGIN, "DataFortress Firewall", 1,  2 },
    { PROCESS_TYPE_CHECK_ORIGIN, "DigiShield InfernoWall", 1,  2 },
    { PROCESS_TYPE_CHECK_ORIGIN, "DigiShield InfernoWall", 3,  2 },
};