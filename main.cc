/*****************************************************************************/
/* Operating-System Construction                                             */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                  M A I N                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* The system's entry point, called from startup.asm after the global        */
/* constructors have been run.                                               */
/*****************************************************************************/

/* INCLUDES */

#include "device/cgastr.h"
#include "user/appl.h"

/* GLOBAL VARIABLES */

// OOStuBS' one and only output stream.  As a global object its constructor
// runs in _init (startup.asm), before main() is entered -- which is what
// clears the screen and homes the cursor.
CGA_Stream kout;

// The (only) application.
Application application;

/* FUNCTIONS */

int main()
{
	application.action();
	return 0;
}
