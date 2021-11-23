/* Log routines */
#include "pros/apix.h"
#include <stdio.h>
#include <stdint.h>
#include "log.h"

/**
 * Macros which are not exported
 */
#define LOG_LEVEL_DEFAULT LOG_LEVEL_WARN

/**
 *  Types which are not exported
 */

/* Log file element type */
typedef struct 
{
    const char * fname; /* File name of the file in question */
    log_level_t level;  /* Level to log the file at */
} log_file_t;

/* Data log element type */
typedef struct
{
    const char * pname; /* Name of the parameter */
    /* Type of parameter (for use with subsequent union) */
    enum
    {
        TYPE_INT,
        TYPE_FLOAT,
        TYPE_STRING
    } ptype;
    /* Data or data pointers for each type */
    union
    {
        int dint;
        double dfloat;
        const char * dstring;
    };
    
} log_element_t;


/* Variables which are exported */
FILE* fd;
FILE* dd;

/* Variables which are not exported */
static char dname[64]; /* File name of the data file and log file */
static char fname[64]; /* The size of these strings is guaranteed by the naming convention */
static log_file_t * lfiles;     /* Pointer to file -> log level array */
static uint32_t lfiles_len;      /* Length of the array (real elements) */
static uint32_t lfiles_alloc;    /* Length of the array allocation */
static log_element_t * deles;   /* Pointer to log element data */
static uint32_t deles_len;      /* Length of the array (real elements) */
static uint32_t deles_alloc;    /* Length of the array allocation */

/* File open/reopen process (called from the task and from initialize */
void log_reopen()
{
    printf("log_reopen called\n");
    /* Variable indicating the uSD was previously inserted */
    static int uSD_last = 0;

    /* File index used for file naming */
    int idx = 0;

    /* Check if uSD is inserted */
    int uSD_avail = usd_is_installed();
    printf("USD status: %d\n",uSD_avail);

    /* If it is now installed and wasn't before, go through the opening process */
    if(uSD_avail && !uSD_last)
    {
        LOG_DEBUG("Opening a new log file, SD was inserted");
        /* Attempt to open the index file to determine the next file name */
        FILE* fidx = fopen("/usd/index.txt","r");

        /* If no error opening index file, read the contents */
        if(fidx)
        {
            /* Scan an integer from the index file */
            int ret = fscanf(fidx,"%d",&idx);
            if(ret)
            {
                /* Successfully read 1 parameter, so idx is valid, increment it
                    * so we use the next available filename
                    */
                idx++;
            }
            /* Close index for now */
            fclose(fidx);
        }
        LOG_INFO("New index is %d",idx);

        /* In any case, reopen the index file to write the latest file index */
        fidx = fopen("/usd/index.txt","w");
        if(fidx)
        {
            /* No error opening file, so write it and close it */
            fprintf(fidx,"%d",idx);
            fclose(fidx);
        }

        /* Determine filenames of the data log and message log */
        sprintf(fname,"/usd/log%06d.txt",idx);
        sprintf(dname,"/usd/data%06d.csv",idx);

        /* Open the new files */
        fd = fopen(fname,"w");
        dd = fopen(dname,"w");

        /* Check for errors in the process */
        if(fd)
        {
            LOG_INFO("Log file opened (%s)",fname);
        }
        else
        {
            LOG_ERROR("Error opening log file (%s)",fname);
        }
        if(dd)
        {
            LOG_INFO("Data file opened (%s)",dname);
        }
        else
        {
            LOG_ERROR("Error opening data file (%s)",dname);
        }

        /* Now that the file is open, we can write the first log entry */
        LOG_INFO("Log Files Opened");
    }
    /* If it was previously installed and isn't any more, close the files and set them null */
    else if(!uSD_avail && uSD_last)
    {
        LOG_WARN("SD now unavailable, closing files");
        fclose(fd);
        fclose(dd);
        fd = NULL;
        dd = NULL;
    }
    /* If we didn't just go through the initial open process, reopen */
    else
    {
        /* Raise the current task priority temporarily so we can ensure that the file handle
            * is null during the reopen, in case any other tasks try to write to the pointer
            * This prevents our task from hogging the CPU during the reopen by
            * raising our priority for the entire duration, while ensuring that the
            * pointer is properly null when we begin the operation
            */
        LOG_DEBUG("About to swap file handles");
        task_set_priority(CURRENT_TASK,TASK_PRIORITY_MAX);
        FILE* fd_temp = fd;
        fd = NULL;
        FILE* dd_temp = dd;
        dd = NULL;
        task_set_priority(CURRENT_TASK,TASK_PRIORITY_DEFAULT-1);
        /* Close the fd and reopen it */
        fclose(fd_temp);
        fd_temp = fopen(fname,"a");
        fclose(dd_temp);
        dd_temp = fopen(dname,"a");
        task_set_priority(CURRENT_TASK,TASK_PRIORITY_MAX);
        fd = fd_temp;
        dd = dd_temp;
        task_set_priority(CURRENT_TASK,TASK_PRIORITY_DEFAULT-1);

        /* Check for errors */
        if(fd)
        {
            LOG_DEBUG("Log file reopened (%s)",fname);
        }
        else
        {
            LOG_ERROR("Error reopening log file (%s)",fname);
        }
        if(dd)
        {
            LOG_DEBUG("Data file reopened (%s)",dname);
        }
        else
        {
            LOG_ERROR("Error reopening data file (%s)",dname);
        }
        LOG_INFO("Log Files Reopened");
    }

    /* Update the last state */
    uSD_last = uSD_avail;
}

/* Logger task - continually attempt to reopen the log files every second */
void log_task(void* param)
{
    LOG_DEBUG("Entering the log task\n");
    do
    {
        log_reopen();
        task_delay(1000);
        LOG_DEBUG("Iterating the log task\n");
    } while (1);
}

/* Initialize the logger */
void log_init()
{
    /* Initialize the lfiles buffer with 16 entries to start */
    lfiles_len = 0;
    lfiles_alloc = 16;
    lfiles = malloc(lfiles_alloc * sizeof(log_file_t));
    if(!lfiles)
    {
        printf("FAILED TO ALLOCATE LOG FILE ARRAY!\n");
        exit(-1);
    }

    /* Initialize the deles buffer with 32 entries to start */
    deles_len = 0;
    deles_alloc = 32;
    deles = malloc(deles_alloc * sizeof(log_element_t));
    if(!deles)
    {
        printf("FAILED TO ALLOCATE DATA ELEMENT ARRAY!\n");
        exit(-1);
    }

    /* Open the logger if the uSD card is inserted */
    log_reopen();

    printf("Attempting to start logger task\n");

    /* Start a new task for the logger to continually check and close/reopen the files */
    task_t ltask = task_create(log_task,NULL,TASK_PRIORITY_DEFAULT,TASK_STACK_DEPTH_DEFAULT,"DataLogger");
    if(!ltask)
    {
        printf("Failed to create task! Got %d\n",errno);
    }
    printf("Task created with state %d\n",task_get_state(ltask));
    fflush(stdout);
    task_delay(5000);
}

/* Function to see if a logged file name exists, and insert it */
static log_level_t log_file_idx(const char * fname)
{
    /* Iterate over the array to find if the file name exists */
    int min = 0;
    int max = lfiles_len-1;
    int iter = 0;
    while(min <= max)
    {
        /* Find the mid point of the two */
        int mid = (min + max) / 2;
        int ret = strcmp(fname,lfiles[mid].fname);
        if(ret < 0)
        {
            max = mid - 1;
        }
        else if(ret > 0)
        {
            min = mid + 1;
        }
        else
        {
            /* They are equal, return this position */
            return mid;
        }
    }

    /* Not found, must insert it at the correct position in the array */
    max++;

    int copylen = lfiles_len - max;

    lfiles_len++;

    /* Check if the array is big enough to hold another element */
    if(lfiles_len >= lfiles_alloc)
    {
        lfiles_alloc = lfiles_alloc + lfiles_alloc;
        lfiles = realloc(lfiles,lfiles_alloc * sizeof(char *));
        if(!lfiles)
        {
            printf("ERROR: Failed to realloc lfiles array\n");
            exit(-1);
        }
    }
    
    /* Need to copy the rest of the array backwards to fit the new element */
    for(int i = 1; i <= copylen; i++)
    {
        lfiles[lfiles_len-i] = lfiles[lfiles_len-i-1];
    }

    /* Insert the new element */
    lfiles[max].fname = fname;
    lfiles[max].level = LOG_LEVEL_DEFAULT;

    /* Return the newly inserted position */
    return max;
    
}

/* Function to set the log level of a specific file */
void log_set_level(const char * fname, log_level_t level)
{

}

/* Internal function to check if a log at level should be printed or not, also prints the log header */
int log_check(const char * fname, const int line,log_level_t level)
{
    /* Log level strings */
    static const char * log_names[] = 
    {
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "ALWAYS"
    };

    /* Clamp level to valid values */
    level = (level > LOG_LEVEL_ALWAYS) ? LOG_LEVEL_ALWAYS : level;

    /* Search through the file array to determine the index of the file */
    int idx = log_file_idx(fname);

    /* If we should not log this file at this level, return false */
    if(lfiles[idx].level > level)
    {
        /* Don't log at this level */
        return 0;
    }


    /* If the log file is not open, return false (can't log to null file) */
    if(!fd)
    {
        printf("FD is invalid\n");
    }

    /* We should log this, so print the log header and then return true to allow the 
     * user message to be printed after
     */

    /* Print the information */
    double time = millis() / 1000.0;
    //fprintf(fd,"\n%5.3f [%s] in %s line %d: ",time,log_names[level],fname,line);
    printf("\n%06.3f [%s] in %s line %d: ",time,log_names[level],fname,line);

    return 0;
}

/* Function to log a data point as a double or int 
 * These functions do NOT assume the name pointer continues to be valid, they instead
 * copy the string
 */
void log_data_dbl(const char * name, const double val);
void log_data_int(const char * name, const int val);

/* Function to log a data point as a string - must be less than 64 chars and not contain any quotations */
void log_data_str(const char * name, const char * val);