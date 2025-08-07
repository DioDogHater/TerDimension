#include "td_input.h"

#ifndef TD_DISABLE_INPUT

// INPUT ENABLED

#include <termios.h>
#include <unistd.h>
#include <errno.h>

static void TD_die(const char* msg){
	perror(msg);
	exit(EXIT_FAILURE);
}

struct termios TD_og_termios;

TD_FUNC void TD_disable_raw_mode(){
	// Restore the original mode
	if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&TD_og_termios) == -1)
		TD_die("tcsetattr (TD_disable_raw_mode)");
}

TD_FUNC void TD_enable_raw_mode() {
	// Store the original (og) mode
	if(tcgetattr(STDIN_FILENO, &TD_og_termios) == -1)
		TD_die("tcgetattr (TD_enable_raw_mode)");

	struct termios raw = TD_og_termios;
	// Disable CTRL-S, CTRL-Q and CTRL-M being turned into \r
	raw.c_iflag &= ~(ICRNL | IXON);

	// Get input, character by character
	// Disables CTRL-C, CTRL-Z and CTRL-V
	// CTRL-C and CTRL-Z are now caught as characters
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

	// Return read() instantly, no matter how much data is held
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 0;

	// Set the new mode
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		TD_die("tcsetattr (TD_enable_raw_mode)");
}

// Character constants for CTRL + * combinations
#define TD_CTRL_C 3
#define TD_CTRL_Z 26
#define TD_CTRL_S 19
#define TD_CTRL_Q 17
#define TD_CTRL_V 22
#define TD_CTRL_O 15
#define TD_ENTER 13

TD_FUNC _Bool TD_get_input(char* c){
	int r = read(STDIN_FILENO,c,1);
	if(r == -1 && errno != EAGAIN)
		TD_die("read (TD_get_input)");
	return (r > 0);
}

#define TD_INPUT_INIT() TD_enable_raw_mode()
#define TD_INPUT_QUIT() TD_disable_raw_mode()

#else

// HANDLE CTRL-C AND CTRL-Z
// A way to handle quitting without errors
// without using the input functions

#include <signal.h>

// Old sigaction for CTRL-C (Interrupting the program)
struct sigaction TD_old_sigint;

// Old sigaction for CTRL-Z (Stopping the program)
struct sigaction TD_old_sigtstp;

// Signal handler for CTRL-C
static void TD_sigint_handler(int sig){
	TD_quit();

	sigaction(SIGINT, &TD_old_sigint, NULL);
	kill(0,SIGINT);
}

static void TD_sigtstp_handler(int sig){
	TD_quit();

	sigaction(SIGTSTP, &TD_old_sigtstp, NULL);
	kill(0,SIGTSTP);
}

TD_FUNC void TD_signal_init(){
	struct sigaction action;

	// Register TD_quit for CTRL-C
	TD_MEMSET(&action, 0, sizeof(action));
	action.sa_handler = &TD_sigint_handler;
	sigaction(SIGINT, &action, &TD_old_sigint);

	// Register TD_quit for CTRL-Z
	TD_MEMSET(&action, 0, sizeof(action));
	action.sa_handler = &TD_sigtstp_handler;
	sigaction(SIGTSTP, &action, &TD_old_sigtstp);
}

#define TD_INPUT_INIT() TD_signal_init()
#define TD_INPUT_QUIT()

#endif