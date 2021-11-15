/* Log routines */
#include "api.h"
#include <stdio.h>
#include <stdint.h>
#include "log.h"


/* Local variables */
FILE* fd;
FILE* dd;
static char dname[64];
static char fname[64];

/* Initialize the logger */
void log_init()
{
    /* Assume the next log file is 0 */
    int idx = 0;

    /* Open the index file to determine the next file name */
    FILE* fidx = fopen("/usd/index.txt","r");
    /* If error, reset index to zero, write a new index file */
    if(!fidx)
    {
        printf("LOG: Error reading index file, rewriting...\n");
    }
    /* Else, read in the index and increment it */
    else
    {
        int ret = fscanf(fidx,"%d",&idx);
        if(!ret)
        {
            printf("LOG: Failed to decode index file, assuming zero!\n");
        }
        else
        {
            idx++;
            printf("LOG: Next log file is %d\n",idx);
        }
        fclose(fidx);
    }

    /* Write back the index, or write it new if it exists */
    fidx = fopen("/usd/index.txt","w");
    if(!fidx)
    {
        printf("LOG: Failed to rewrite index file\n");
    }
    else
    {
        fprintf(fidx,"%d",idx);
        fclose(fidx);
    }

    /* Open the log file */
    sprintf(fname,"/usd/log%06d.txt",idx);
    fd = fopen(fname,"w");
    if(!fd)
    {
        printf("LOG: Failed to open data file\n");
    }

    /* Now open a data file */
    sprintf(dname,"/usd/data%06d.csv",idx);
    dd = fopen(dname,"w");
    if(!dd)
    {
        printf("LOG: Failed to open data file\n");
    }
    else
    {
        /* Start header with 'time' column */
        fprintf(dd,"time");
    }
}

/* Log step function, called periodically automatically */
void log_step()
{
    /* Start a new line in the log */
    if(dd)
    {
        fprintf(dd,"\n%08d",pros::millis());
    }

    /* Flush the log buffer every second, and the data buffer on the subsequent call to reduce jitter */
    static uint32_t lmlog;
    static int nextcall = 0;

    /* Reopen data if the log reopened last call */
    if(nextcall)
    {
        if(dd)
        {
            fclose(dd);
            dd = fopen(dname,"a");
        }
        nextcall = 0;
    }

    /* Reopen log on every 1000 interval */
    if((pros::millis() - lmlog) > 1000)
    {
        lmlog = pros::millis();
        nextcall = 1;
        if(fd)
        {
            fclose(fd);
            fd = fopen(fname,"a");
        }
    }
}

/* Function to name the next parameter */
void log_param(const char * name)
{
    /* Write parameter to logfile */
    LOG_PRINT("LOGGER adding a new parameter \'%s\'",name);
    /* Also append to header of csv */
    if(dd)
    {  
        fprintf(dd,",%s",name);
    }
}

/* Function to log data */
void log_data(int numInt,int * paramInt,int numDbl,double * paramDbl)
{
    /* If file is currently valid */
    if(dd)
    {
        for(int i = 0; i < numInt; i++)
        {
            fprintf(dd,",%d",paramInt[i]);
        }
        for(int i = 0; i < numDbl; i++)
        {
            fprintf(dd,",%f",paramDbl[i]);
        }
    }
}