/*****************************************************************************
* | File        :   sysinfo.c
* | Function    :   CPU temperature / usage, memory usage, IP address
******************************************************************************/
#include "sysinfo.h"
#include <stdio.h>
#include <string.h>
#include <glob.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

double SYS_GetTemp(void)
{
    glob_t g;
    double max = -1000;
    size_t i;

    if (glob("/sys/class/thermal/thermal_zone*/temp", 0, NULL, &g) != 0)
        return -1;
    for (i = 0; i < g.gl_pathc; i++) {
        FILE *f = fopen(g.gl_pathv[i], "r");
        long t;
        if (!f)
            continue;
        if (fscanf(f, "%ld", &t) == 1 && t / 1000.0 > max)
            max = t / 1000.0;
        fclose(f);
    }
    globfree(&g);
    return max;
}

int SYS_GetCPUUsage(void)
{
    static unsigned long long prev_idle, prev_total;
    unsigned long long user, nice, sys, idle, iowait, irq, softirq, steal;
    unsigned long long idle_all, total, d_idle, d_total;
    FILE *f = fopen("/proc/stat", "r");
    int n;

    if (!f)
        return -1;
    n = fscanf(f, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
               &user, &nice, &sys, &idle, &iowait, &irq, &softirq, &steal);
    fclose(f);
    if (n != 8)
        return -1;

    idle_all = idle + iowait;
    total = user + nice + sys + idle + iowait + irq + softirq + steal;
    d_idle = idle_all - prev_idle;
    d_total = total - prev_total;
    prev_idle = idle_all;
    prev_total = total;
    if (d_total == 0)
        return 0;
    return (int)(100 * (d_total - d_idle) / d_total);
}

int SYS_GetMemUsage(void)
{
    FILE *f = fopen("/proc/meminfo", "r");
    char line[128];
    long total = 0, avail = 0;

    if (!f)
        return -1;
    while (fgets(line, sizeof(line), f)) {
        sscanf(line, "MemTotal: %ld kB", &total);
        sscanf(line, "MemAvailable: %ld kB", &avail);
    }
    fclose(f);
    if (total <= 0)
        return -1;
    return (int)(100 * (total - avail) / total);
}

int SYS_GetIP(char *buf, size_t len)
{
    static const char *skip[] = {"lo", "docker", "br-", "veth", "virbr"};
    struct ifaddrs *ifaddr, *ifa;
    int found = -1;
    size_t i;

    if (getifaddrs(&ifaddr) == -1)
        return -1;
    for (ifa = ifaddr; ifa != NULL && found != 0; ifa = ifa->ifa_next) {
        int skipped = 0;
        if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_INET)
            continue;
        for (i = 0; i < sizeof(skip) / sizeof(skip[0]); i++)
            if (strncmp(ifa->ifa_name, skip[i], strlen(skip[i])) == 0)
                skipped = 1;
        if (skipped)
            continue;
        if (getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                        buf, len, NULL, 0, NI_NUMERICHOST) == 0)
            found = 0;
    }
    freeifaddrs(ifaddr);
    return found;
}
