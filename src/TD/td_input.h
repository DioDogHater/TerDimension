#ifndef TERDIMENSION_INPUT_H
#define TERDIMENSION_INPUT_H

#include "td_definitions.h"

#ifndef TD_DISABLE_INPUT

// INPUT ENABLED

#include <termios.h>
#include <unistd.h>
#include <errno.h>

static void TD_die(const char* msg);

extern struct termios TD_og_termios;

TD_FUNC void TD_disable_raw_mode();

TD_FUNC void TD_enable_raw_mode();

// Character constants for CTRL + * combinations
#define TD_CTRL_C 3
#define TD_CTRL_Z 26
#define TD_CTRL_S 19
#define TD_CTRL_Q 17
#define TD_CTRL_V 22
#define TD_CTRL_O 15
#define TD_ENTER 13

TD_FUNC _Bool TD_get_input(char*);

#define TD_INPUT_INIT() TD_enable_raw_mode()
#define TD_INPUT_QUIT() TD_disable_raw_mode()

#else

// HANDLE CTRL-C AND CTRL-Z
// A way to handle quitting without errors
// without using the input functions

#include <signal.h>

// Old sigaction for CTRL-C (Interrupting the program)
extern struct sigaction TD_old_sigint;

// Old sigaction for CTRL-Z (Stopping the program)
extern struct sigaction TD_old_sigtstp;

// Signal handler for CTRL-C
static void TD_sigint_handler(int);

static void TD_sigtstp_handler(int);

TD_FUNC void TD_signal_init();

#define TD_INPUT_INIT() TD_signal_init()
#define TD_INPUT_QUIT()

#endif

#endif
