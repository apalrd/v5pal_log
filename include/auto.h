/* Autonomous selector system
 * 
 * First, create an array of all autonomous programs as shown in the example
 * 
 * Next, call auto_picker(auton_list, auton_list_length) in Initialize to start the process
 * This will display a selector on the screen
 * 
 * When autonomous() is called, call auto_run() to execute the selected autonomous
 */

#ifndef _AUTO_H_
#define _AUTO_H_

#pragma GCC diagnostic ignored "-Wwrite-strings"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h>

/* Color enumeration */
typedef enum
{
    AUTO_COLOR_RED = 1,
    AUTO_COLOR_SKILLS = 0,
    AUTO_COLOR_BLUE = -1
} auto_color_t;

/* Starting position enumeration */
typedef enum
{
    AUTO_POS_1,
    AUTO_POS_2,
    AUTO_POS_SKILLS
} auto_pos_t;

/* Function prototype for autonomous function must be:
 * void auto(auto_color_t color, auto_pos_t position)
 */
typedef void(*auto_func_t)(auto_color_t,auto_pos_t);

/* Autonomous routine definition */
typedef struct
{
    auto_func_t function;
    auto_pos_t position;
    char * name;
} auto_routine_t;


/* Function to run the auto picker */
void auto_picker(const auto_routine_t * list, size_t length);

/* Function to run the picked autonomous */
void auto_run(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _AUTO_H_ */