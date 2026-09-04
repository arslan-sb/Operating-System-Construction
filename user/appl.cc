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
/* Add your code here */ 
 
/* GLOBAL VARIABLES */

extern CGA_Stream kout;
/* Add your code here */ 
 
void Application::action()
{
/* Add your code here */ 
kout << "Test          <stream result> -> <expected>" << endl;
kout << "zero:         " << 0 << " -> 0" << endl;
kout << "decimal:      " << dec << 42 << " -> 42" << endl;
kout << "binary:       " << bin << 42 << dec << " -> 0b101010" << endl;
kout << "octal:        " << oct << 42 << dec << " -> 052" << endl;
kout << "hex:          " << hex << 42 << dec << " -> 0x2a" << endl;
kout << "uint64_t max: " << ~((unsigned long)0) << " -> 18446744073709551615" << endl;
kout << "int64_t max:  " << ~(1l<<63) << " -> 9223372036854775807" << endl;
kout << "int64_t min:  " << (1l<<63) << " -> -9223372036854775808" << endl;
kout << "some int64_t: " << (-1234567890123456789) << " -> -1234567890123456789" << endl;
kout << "some int64_t: " << (1234567890123456789) << " -> 1234567890123456789" << endl;
kout << "pointer:      " << reinterpret_cast<void*>(1994473406541717165ul) << " -> 0x1badcafefee1dead" << endl;
kout << "smiley:       " << static_cast<char>(1) << endl;
 
}
