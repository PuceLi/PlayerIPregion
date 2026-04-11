#ifndef BASE_H
#define BASE_H

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <string>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

namespace xdb {
using std::string;

constexpr int ipv4 = 4;
constexpr int ipv6 = 6;
constexpr int policy_file    = 0;
constexpr int policy_vector  = 1;
constexpr int policy_content = 2;
constexpr int length_header = 256;
constexpr int length_vector = 256 * 256 * 8;

extern int ip_version;
extern int ip_size;
extern int content_size;

void init_xdb(int version);
void log_exit(const string &msg);
void read_bin(int index, char *buf, size_t len, FILE *db);

unsigned to_uint(const char *buf);
unsigned to_ushort(const char *buf);
unsigned to_int(const char *buf, int n);

unsigned long long get_time();
}
#endif