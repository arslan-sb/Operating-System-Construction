/*****************************************************************************/
/* Operating-System Construction                                             */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                         C G A _ S T R E A M                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* The CGA_Stream class allows to print different data types as text strings */
/* to a PC's CGA screen.                                                     */
/* For attributes/colors and cursor positioning use the methods of class     */
/* CGA_Screen.                                                               */
/*****************************************************************************/

#include "device/cgastr.h"

CGA_Stream::CGA_Stream() : attribute(CGA_Screen::STD_ATTR)
{
	// The CGA_Screen base class constructor has already cleared the
	// screen and homed the cursor.
}

void CGA_Stream::flush()
{
	// buffer and pos are the protected members inherited from
	// Stringbuffer; print() comes from CGA_Screen.
	CGA_Screen::print(buffer, pos, attribute);
	pos = 0;
}
