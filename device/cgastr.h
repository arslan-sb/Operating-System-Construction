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

#ifndef __cgastr_include__
#define __cgastr_include__

#include "object/o_stream.h"
#include "machine/cgascr.h"

// Multiple inheritance: the formatting and buffering come from O_Stream, the
// hardware access from CGA_Screen.  CGA_Stream is exactly the place where the
// device-independent half of the output path meets the device-dependent one,
// which is why it is the only class here that can implement flush().
class CGA_Stream : public O_Stream, public CGA_Screen {
public:
	CGA_Stream(CGA_Stream &copy) = delete; // prevent copying
	CGA_Stream& operator=(const CGA_Stream&) = delete; // prevent assignment

	CGA_Stream();

	// FLUSH: writes the buffered characters to the screen and empties the
	//        buffer.
	void flush() override;

	// Attribute (color) used for subsequent output.  Public so that an
	// application can change the color between two outputs.
	unsigned char attribute;
};

#endif
