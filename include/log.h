#ifndef LOG_H
#define LOG_H
/* Logger functions */
#include <stdio.h>
extern FILE* fd;
extern FILE* dd;

/* log print functions */
#define LOG_PRINT(...) if(fd){fprintf(fd,"\n%08d: ",pros::millis());fprintf(fd,__VA_ARGS__);}
/* Functions for the logger */
void log_init();
void log_param(const char * name);
void log_data(int numInt,int * paramInt,int numDbl,double * paramDbl);
void log_step();

#endif