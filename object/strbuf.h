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

#ifndef __strbuf_include__
#define __strbuf_include__

class Stringbuffer {
public:
	Stringbuffer(const Stringbuffer &copy) = delete; // prevent copying
	Stringbuffer& operator=(const Stringbuffer&) = delete; // prevent assignment

	// FLUSH: hands the buffered characters to the output device and
	//        empties the buffer.
	//        Pure virtual, because Stringbuffer is device independent: it
	//        collects characters but has no idea where they should end up.
	//        The derived class supplies the device knowledge.
	virtual void flush() = 0;

protected:
	// Only derived classes may construct a Stringbuffer -- on its own it
	// is useless, since flush() has no implementation.
	Stringbuffer();

	// PUT: appends one character, flushing automatically when the buffer
	//      runs full.
	void put(char c);

	// One CGA line.  Larger buffers would delay output without buying
	// anything, since flush() writes at most one line's worth of
	// characters in one go anyway; smaller ones would cause more device
	// accesses than necessary.
	enum { BUFFER_SIZE = 80 };

	// protected rather than private: flush() lives in the derived class
	// and has to read what we collected.
	char buffer[BUFFER_SIZE];
	int pos; // next free slot == number of buffered characters
};

#endif
