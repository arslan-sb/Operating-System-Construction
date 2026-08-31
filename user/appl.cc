/*****************************************************************************/
/* Operating-System Construction                                             */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                         A P P L I C A T I O N                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* The Application class defines the (only) application for OOStuBS.         */
/*****************************************************************************/

/* INCLUDES */

#include "user/appl.h"
#include "device/cgastr.h"

/* GLOBAL VARIABLES */

extern CGA_Stream kout;

/* METHODS */

void Application::action()
{
	kout << "OOStuBS -- Lab 1: CGA output" << endl;
	kout << "----------------------------" << endl << endl;

	// number bases, selected via the manipulators
	int value = 42;
	kout << "42 dec:  " << dec << value << endl;
	kout << "42 hex:  " << hex << value << endl;
	kout << "42 oct:  " << oct << value << endl;
	kout << "42 bin:  " << bin << value << endl << dec << endl;

	// signed values: '-' in base 10, the bit pattern in every other base,
	// at the width of the original type
	kout << "-1 dec:  " << dec << -1 << endl;
	kout << "-1 hex:  " << hex << -1 << endl << dec;
	kout << "min int: " << (int)(-2147483647 - 1) << endl << endl;

	// the remaining supported types
	kout << "bool:    " << true << endl;
	kout << "char:    " << 'X' << endl;
	kout << "ptr:     " << (void *)&value << endl << endl;

	// a line longer than 80 characters, to show the wrap in print()
	kout << "wrap:    ";
	for (int i = 0; i < 90; i++)
		kout << (char)('0' + i % 10);
	kout << endl;
}
