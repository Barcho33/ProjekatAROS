#include <stdio.h>
#include "proclib.h"


void initProc(PROC *proc){
    getTTY(proc->pid, proc);
    getCMD(proc->pid, proc);
    getUSR(proc->pid, proc);              
    getStartTime(proc->pid, proc);
    getStatus(proc->pid, proc);
    getCPUUsage(proc->pid, proc);
    getMEM(proc->pid, proc);
    getRSS(proc->pid, proc);
    getVSZ(proc->pid, proc);

}
void getCMD(char pid[], PROC *proc){
    
    char putanja[1024];
    snprintf(putanja, sizeof(putanja), "/proc/%s/cmdline", pid);

    FILE *file = fopen(putanja, "r");
    if (file) {
 
        fscanf(file, "%s", proc->cmd);
        fclose(file);

        
    }else printf("Greska prilikom otvaranja fajla!");

}
void getUSR(char pid[], PROC *proc){
    char putanja[256];
    char usr[256];

    snprintf(putanja, sizeof(putanja), "/proc/%s/status", pid);

    FILE *file = fopen(putanja, "r");
    if (file) {

        char line[256];

        while(fgets(line, sizeof(line), file) != NULL)
            if(strstr(line, "Uid:") != NULL){
                strcpy(usr, line);
                break;
            }
        fclose(file);

        }else printf("Nije moguce otvoriti datoteku!");
    
    int first;

    if (sscanf(usr, "Uid: %d %*d %*d %*d", &first) == 1){

        struct passwd *pwd = getpwuid(first);

        if(pwd != NULL)
            snprintf(proc->usr, sizeof(proc->usr), "%s", pwd->pw_name);

    }

}
void getStatus(char pid[], PROC *proc) {
    char putanja[1024];
    snprintf(putanja, sizeof(putanja), "/proc/%s/stat", pid);

    FILE *file = fopen(putanja, "r");
    if (file) {
        char stat;
        
        for (int i = 0; i < 2; ++i) {
            fscanf(file, "%*s");
        }

      
        if (fscanf(file, " %c", &stat) == 1);
        else {
            printf("Greska prilikom citanja STAT iz stat fajla.\n");
        }

        fclose(file);
        proc->stat = stat;
    } else {
        printf("Greska prilikom otvaranja stat fajla");
    }
}
void getTTY(char pid[], PROC *proc) {
    char putanja[1024];
    snprintf(putanja, sizeof(putanja), "/proc/%s/stat", pid);

    FILE *file = fopen(putanja, "r");
    if (file) {
        long tty;  
   
        if (fscanf(file, "%*d %*s %*c %*d %*d %*d %ld", &tty) == 1);
        else {
            snprintf(proc->tty, sizeof(proc->tty),"?");
        }

        fclose(file);
        convertTTYnum(tty, proc);
    } else {
        printf("Greska prilikom otvaranja stat fajla");
    }
}
void getStartTime(char pid[], PROC *proc){
    char putanja[1024];
    snprintf(putanja, sizeof(putanja), "/proc/%s/stat", pid);

    FILE *file = fopen(putanja, "r");
    if (file) {
        unsigned long long  startTime;
        unsigned long long ticks_per_second = sysconf(_SC_CLK_TCK);
        double uptime = getUpTime();

        for(int i = 0; i < 21; ++i)
            fscanf(file, "%*s");

        if (fscanf(file, "%llu", &startTime) == 1){
            double startTime_sec = (double)startTime/ticks_per_second;

            int hours = (int)(startTime_sec / 3600) % 24;
            int minutes = (int)(startTime_sec / 60) % 60;
            

            snprintf(proc->startTime, sizeof(proc->startTime), "%02d:%02d", hours, minutes);
            
            fclose(file);
        }else printf("GRESKA");
        

    }else printf("Greska prilikom otvaranja fajla!");
}
void convertTTYnum(long tty_n, PROC *proc){

    int major = MAJOR(tty_n);
    int minor = MINOR(tty_n);

    if(major == 4){
        if(minor >= 0 && minor <= 63)
            snprintf(proc->tty, sizeof(proc->tty), "tty%d", minor);
        else if(minor > 63 && minor <= 255)
            snprintf(proc->tty, sizeof(proc->tty), "ttyS%d", minor - 64);
        else
            snprintf(proc->tty, sizeof(proc->tty), "?");
    }
    else if (major == 5 && minor == 0)
        snprintf(proc->tty, sizeof(proc->tty), "tty");
    else if (major >= 136 && major <= 143)
        snprintf(proc->tty, sizeof(proc->tty), "pts/%d", minor);
    else
        snprintf(proc->tty, sizeof(proc->tty), "?");
}
void getCPUUsage(char pid[], PROC *proc)
{
    char putanja[1024];
    snprintf(putanja, sizeof(putanja), "/proc/%s/stat", pid);

    FILE *file = fopen(putanja, "r");
    if (file)
    {
        unsigned long utime, stime;
        unsigned long long startTime;
        char content[1000];
        fgets(content, 1000, file); 
        char *str = strtok(content, ")");
        int i = 2;
        while (str != NULL)
        {
            str = strtok(NULL, " ");
            i++;
            if (i == 14)
            {
                utime = strtol(str, NULL, 10);
            }
            else if (i == 15)
            {
                stime = strtol(str, NULL, 10);
            }
            else if (i == 22)
            {
                startTime = strtol(str, NULL, 10);
            }
        }

        long long ticks_per_second = sysconf(_SC_CLK_TCK);

        double startTime_sec = (double)startTime / ticks_per_second;
        double utime_sec = (double)utime / ticks_per_second;
        double stime_sec = (double)stime / ticks_per_second;
        double elapsed_time = (getUpTime()) - startTime_sec;
        double cpu_usg = ((utime_sec + stime_sec) / elapsed_time) * 100;
    
        proc->cpu = cpu_usg;
    }
}
double getUpTime(){

    char* putanja = "/proc/uptime";
    
    FILE *file = fopen(putanja, "r");

    if(file){

        double uptime;
        fscanf(file, "%lf", &uptime);

        fclose(file);

        return uptime;

    }else printf("Nije moguce otvoriti uptime fajl!\n");
}
void getRSS(char pid[], PROC *proc){
    char putanja[1024];
    snprintf(putanja, sizeof(putanja), "/proc/%s/status", pid);

    FILE *file = fopen(putanja, "r");
    if (file) {
        char line[256];
        int status = 0;
        unsigned long rss;
        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, "VmRSS:")) {
                status = 1;
                
                if (sscanf(line, "VmRSS: %lu", &rss) == 1);
                else {
                    printf("Greska prilikom citanja VmRSS.\n");
                }
                break; 
            }
        }
        if(!status)
            rss = 0;

        fclose(file);
        proc->rss = rss;
    } else {
        printf("Greska prilikom otvaranja status fajla!\n");
    }
}
void getVSZ(char pid[], PROC *proc) {
    char putanja[1024];
    snprintf(putanja, sizeof(putanja), "/proc/%s/status", pid);
    unsigned long vsz;
    FILE *file = fopen(putanja, "r");
    if (file) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, "VmSize:")) {
                
                if (sscanf(line, "VmSize: %lu", &vsz) == 1);
                else {
                    printf("Greska prilikom citanja VSZ.\n");
                }
                break;  
            }
        }

        fclose(file);
        proc->vsz = vsz;
    
    } else {
        printf("Greska prilikom otvaranja status fajla!\n");
        
    }
}
void getMEM(char pid[], PROC *proc) {
    char putanja[1024];
    snprintf(putanja, sizeof(putanja), "/proc/%s/status", pid);

    FILE *file = fopen(putanja, "r");
    if (file) {

        char line[256];
        int status = 0;
        double mem_percentage;
        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, "VmRSS:")) {
                status = 1;
                unsigned long rss;
                if (sscanf(line, "VmRSS: %lu", &rss) == 1) {
                    mem_percentage = ((double)rss / getTotalMemory()) * 100.0;
                } else {
                    printf("Greska prilikom citanja VmRSS.\n");
                }
                break;  
            }
        }
         if(!status)
            mem_percentage = 0.0;

        fclose(file);
        proc->mem = mem_percentage;
       
    } else {
        printf("Greska prilikom otvaranja status fajla!\n");
    }

}
unsigned long getTotalMemory() {
    struct sysinfo info;
    sysinfo(&info);
    return info.totalram * info.mem_unit / 1024;  
}
