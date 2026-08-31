/*****************************************************************************/
/* Operating-System Construction                                             */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                            O _ S T R E A M                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* The O_Stream class defines the << operator for several pre-defined data   */
/* types and thereby realizes output functionality similar to C++'s iostream */
/* library. By default, this class supports printing characters, strings and */
/* integer numbers of various bit sizes. Another << operator allows to use   */
/* so-called 'manipulators'.                                                 */
/*                                                                           */
/* Besides class O_Stream this file also defines the manipulators hex, dec,  */
/* oct and bin for choosing the basis in number representations, and endl    */
/* for implementing an (implicitly flushing) line termination.               */
/*****************************************************************************/

#ifndef __o_stream_include__
#define __o_stream_include__

#include "object/strbuf.h"

// O_Stream inherits the character collecting from Stringbuffer and adds the
// formatting on top.  It stays abstract: flush() is still not implemented,
// because O_Stream is as device independent as its base class.
class O_Stream : public Stringbuffer {
public:
	O_Stream(const O_Stream &copy) = delete; // prevent copying
	O_Stream& operator=(const O_Stream&) = delete; // prevent assignment

	O_Stream();

	// The number system used for integer output.  Public so that the
	// manipulators below -- which are plain functions, not members -- can
	// change it.
	int base;

	// Every operator returns a reference to the stream itself, which is
	// what makes the calls chainable:  kout << "x = " << 42 << endl;
	O_Stream& operator<<(char c);
	O_Stream& operator<<(unsigned char c);
	O_Stream& operator<<(const char *string);
	O_Stream& operator<<(bool b);

	O_Stream& operator<<(short value);
	O_Stream& operator<<(unsigned short value);
	O_Stream& operator<<(int value);
	O_Stream& operator<<(unsigned int value);
	O_Stream& operator<<(long value);
	O_Stream& operator<<(unsigned long value);
	O_Stream& operator<<(long long value);
	O_Stream& operator<<(unsigned long long value);

	O_Stream& operator<<(void *pointer);

	// MANIPULATORS: takes a function with the signature below and applies
	//               it to this stream.  This is the whole trick behind
	//               being able to write 'kout << hex' -- 'hex' is not a
	//               value, it is a function pointer.
	O_Stream& operator<<(O_Stream& (*fkt)(O_Stream&));

private:
	// PUT_UNSIGNED: writes value's digits in the currently selected base.
	void put_unsigned(unsigned long long value);

	// PUT_SIGNED: writes a leading '-' for negative values, then the
	//             magnitude.  Only used for base 10.
	void put_signed(long long value);
};

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                          M A N I P U L A T O R S                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* The functions below all receive and return a reference to an O_Stream     */
/* object. Because class O_Stream defines an operator<< for functions with   */
/* this signature, they can be called in this operator's implementation and  */
/* even be embedded in a "stream" of outputs. A manipulator's task is to     */
/* influence subsequent output, e.g. by switching to a different number      */
/* system.                                                                   */
/*---------------------------------------------------------------------------*/

// ENDL: inserts a newline in the output and flushes the buffer
O_Stream& endl(O_Stream& os);

// BIN: selects the binary number system
O_Stream& bin(O_Stream& os);

// OCT: selects the octal number system
O_Stream& oct(O_Stream& os);

// DEC: selects the decimal number system
O_Stream& dec(O_Stream& os);

// HEX: selects the hexadecimal number system
O_Stream& hex(O_Stream& os);

#endif
