/*****************************************************************************/
/* Operating-System Construction                                             */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                       S T R I N G B U F F E R                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* The Stringbuffer class provides a buffer for collecting characters to be  */
/* printed to an output device, in our case the PC screen. The actual output */
/* occurs once the buffer is full, or when the user explicitly calls         */
/* flush().                                                                  */
/* As Stringbuffer is intended to be device independent, flush() is a        */
/* (pure) virtual method that must be defined by derived classes.            */
/*****************************************************************************/

#include "object/strbuf.h"

Stringbuffer::Stringbuffer() : pos(0)
{
	// Careful: flush() must NOT be called from here.  During the base
	// class constructor the vtable still points at Stringbuffer, so the
	// call would land on the pure virtual method.
}

void Stringbuffer::put(char c)
{
	buffer[pos++] = c;

	if (pos == BUFFER_SIZE)
		flush();
}
