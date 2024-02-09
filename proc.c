#include <stdio.h>
#include "proclib.h"

int main (int argc, char *argv[]){

    DIR *dir = opendir("/proc");

    if(dir){
        
      struct dirent *pdir;
      printf("\x1b[31m%-16s %-5s %-4s %-4s  %-10s  %-6s %-5s %-4s %-5s %s\x1b[0m\n", "USER", "PID", "%CPU","%MEM", "VSZ", "RSS", "TTY", "STAT","TIME", "COMMAND");
        while((pdir = readdir(dir)) != NULL)
            if (atoi(pdir->d_name) != 0) {
                PROC temp;
                strcpy(temp.pid, pdir->d_name);
                initProc(&temp);

                printf("%-16s %-5s %4.1lf  %.1f  %-10lu  %-6lu %-5s %-4c %s %s\n",temp.usr,temp.pid, temp.cpu,temp.mem, temp.vsz,
                           temp.rss, temp.tty, temp.stat, temp.startTime, temp.cmd);

            }

        closedir(dir);
    }
    
    else printf("Nije moguce otvoriti direktorijum /proc !");

    return 0;
}
