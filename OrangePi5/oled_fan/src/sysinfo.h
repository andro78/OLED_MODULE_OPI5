#ifndef _SYSINFO_H_
#define _SYSINFO_H_

#include <stddef.h>

double SYS_GetTemp(void);                  // hottest thermal zone, C
int    SYS_GetCPUUsage(void);              // %, since previous call
int    SYS_GetMemUsage(void);              // %, (total - available) / total
int    SYS_GetIP(char *buf, size_t len);   // first external IPv4, 0 on success

#endif
