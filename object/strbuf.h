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
/* Add your code here */ 
	virtual void flush() = 0;

	protected:  //buffer and pos are protected, not private, because flush() lives in the derived class and has to read what we collected
	Stringbuffer();
	void put(char c);
	enum { BUFFER_SIZE =80};
	char buffer[BUFFER_SIZE];
	int pos;

};

#endif
