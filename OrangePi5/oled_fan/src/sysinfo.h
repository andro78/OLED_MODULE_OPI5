#ifndef _SYSINFO_H_
#define _SYSINFO_H_

#include <stddef.h>

double SYS_GetTemp(void);                  // hottest thermal zone, C
int    SYS_GetCPUUsage(void);              // %, since previous call
int    SYS_GetMemUsage(void);              // %, (total - available) / total
int    SYS_GetIfaceIP(const char *const *prefixes, char *buf, size_t len); // IPv4 of the first interface matching a prefix

#endif
