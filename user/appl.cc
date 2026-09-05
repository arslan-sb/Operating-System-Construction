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
#include "machine/keyctrl.h"
/* Add your code here */ 
 
/* GLOBAL VARIABLES */

extern CGA_Stream kout;
/* Add your code here */ 
 
void Application::action()
{
    kout << "OOStuBS -- Task 1" << endl;
    kout << "=================" << endl << endl;

    // --- number bases -----------------------------------------------------
    int value = 42;
    kout << "42 dec:  " << dec << value << endl;
    kout << "42 hex:  " << hex << value << endl;
    kout << "42 oct:  " << oct << value << endl;
    kout << "42 bin:  " << bin << value << endl << dec << endl;

    // --- 64-bit values ----------------------------------------------------
    long long          smin = -9223372036854775807LL - 1;   // LLONG_MIN
    unsigned long long umax = 18446744073709551615ULL;      // ULLONG_MAX

    kout << "int64  min: " << smin << endl;
    kout << "uint64 max: " << umax << endl;
    kout << "uint64 max: " << hex << umax << " (hex)" << endl << dec;

    // sign handling: '-' in base 10, bit pattern in every other base
    kout << "-1 dec:     " << -1 << endl;
    kout << "-1 hex:     " << hex << -1 << endl << dec << endl;

    // --- pointers and characters ------------------------------------------
    kout << "&value:     " << (void *)&value << endl;
    kout << "*(&value):  " << *(&value) << endl;
    kout << "char:       " << 'X' << endl;
    kout << "bool:       " << true << endl << endl;

    // --- keyboard ---------------------------------------------------------
    Keyboard_Controller keyboard;

    kout << "type something:" << endl << endl;

    while (true) {
        Key key = keyboard.key_hit();

        if (key.valid())            // false for modifier-only presses
            kout << (char)key.ascii() << flush;
    }
}
