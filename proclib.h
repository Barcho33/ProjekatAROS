#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <time.h>
#include <utmp.h>
#include <linux/kdev_t.h>
#include <sys/sysinfo.h>

#ifndef PROCLIB_H
#define PROCLIB_H

struct proc
{
    char pid[16];
    char cmd[1024];
    char tty[32];
    char startTime[16];
    char usr[16];
    double cpu;
    double mem;
    unsigned long vsz;
    unsigned long rss;
    char stat;

};
typedef struct proc PROC;

void initProc(PROC *proc);
void getCMD(char pid[], PROC *proc);
void getUSR(char pid[], PROC *proc);
void getStatus(char pid[], PROC *proc);
void getTTY(char pid[], PROC *proc);
void getStartTime(char pid[], PROC *proc);
void convertTTYnum(long tty_n, PROC *proc);
void getCPUUsage(char pid[], PROC *proc);
double getUpTime();
void getRSS(char pid[], PROC *proc);
void getVSZ(char pid[], PROC *proc);
void getMEM(char pid[], PROC *proc);
unsigned long getTotalMemory();

#endif 