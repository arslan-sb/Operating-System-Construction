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

/* Add your code here */ 

CGA_Stream::CGA_Stream():attribute(CGA_Screen::STD_ATTR){}

void CGA_Stream::flush()
{
    CGA_Screen::print(buffer, pos, attribute);
    pos = 0;
}

