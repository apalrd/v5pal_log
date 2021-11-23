#ifndef _LOG_H_
#define _LOG_H_


#ifdef __cplusplus
extern "C" {
#endif


/* Headers required by log.h macros */
#include <stdio.h>

/* File descriptors for logfiles
 * fd is the log file descriptor
 * dd is the data file descriptor
 * Not to be used directly, but used by macros in this header
 */
extern FILE* fd;
extern FILE* dd;

/* Log Verbosity Level enumeration */
typedef enum
{
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_ALWAYS
} log_level_t;


/**
 *  Functions which are exported by the logger module
 **/


/* Functions to print a message at the specified log levels
 * The funtion will print if the given file is set to log at or above this level
 */
#define LOG_ALWAYS(...) do{if(log_check(__FILE__,__LINE__,LOG_LEVEL_ALWAYS)){fprintf(fd,__VA_ARGS__);printf(__VA_ARGS__);}}while(0)
#define LOG_ERROR(...) do{if(log_check(__FILE__,__LINE__,LOG_LEVEL_ERROR)){fprintf(fd,__VA_ARGS__);printf(__VA_ARGS__);}}while(0)
#define LOG_WARN(...) do{if(log_check(__FILE__,__LINE__,LOG_LEVEL_WARN)){fprintf(fd,__VA_ARGS__);printf(__VA_ARGS__);}}while(0)
#define LOG_INFO(...) do{if(log_check(__FILE__,__LINE__,LOG_LEVEL_INFO)){fprintf(fd,__VA_ARGS__);printf(__VA_ARGS__);}}while(0)
#define LOG_DEBUG(...) do{if(log_check(__FILE__,__LINE__,LOG_LEVEL_DEBUG)){fprintf(fd,__VA_ARGS__);printf(__VA_ARGS__);}}while(0)

/* Initialize the logger module, it then operates from its own task */
void log_init();

/* Function to set the log level of a specific file
 * The pointer passed is assumed to be valid in global scope once the calling function returns
 * i.e. it assumes a string literal constant
 */
void log_set_level(const char * fname, log_level_t level);

/* Internal function to check if a log at level should be printed or not, also prints the log header
 * fname is assumed to be a string literal, as it should be a C-string defined by __FILE__
 * The pointer passed is assumed to be valid in global scope once the calling function returns
 */
int log_check(const char * fname, const int line,log_level_t level);

/* Function to log a data point as a double or int 
 * These functions do NOT assume the name pointer continues to be valid, they instead
 * copy the string
 */
void log_data_dbl(const char * name, const double val);
void log_data_int(const char * name, const int val);

/* Function to log a data point as a string - must be less than 64 chars and not contain any quotations */
void log_data_str(const char * name, const char * val);

#ifdef __cplusplus
}
#endif

#endif /* _LOG_H_ */