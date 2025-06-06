#include <stdio.h>
#include <math.h>

#define TD_TERMINAL

#include "terdimension.h"

int main(void){
	if(!TD_init(100,100))
		return 1;

	TD_quit();
}