#ifndef __screen_include__
#define __screen_include__

/*****************************************************************************/
/* Operating-System Construction                                             */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                             C G A _ S C R E E N                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* This class allows accessing the PC's screen.  Accesses work directly on   */
/* the hardware level, i.e. via the video memory and the graphic adapter's   */
/* I/O ports.                                                                */
/*****************************************************************************/

#include "machine/io_port.h"

class CGA_Screen {
public:
	CGA_Screen(const CGA_Screen &copy) = delete; // prevent copying
	CGA_Screen& operator=(const CGA_Screen&) = delete; // prevent assignment

	// screen geometry of the CGA text mode
	enum { COLUMNS = 80, ROWS = 25 };

	// Attribute byte:  bit 7   blink
	//                  bit 6-4 background color
	//                  bit 3-0 foreground color
	enum { STD_ATTR = 0x0f }; // bright white on black

	// CGA_SCREEN: clears the screen and puts the cursor into the top left
	//             corner.
	CGA_Screen();

	// SHOW: writes character c with attribute attrib to position x/y.
	//       Out-of-range coordinates are ignored.
	void show(int x, int y, char c, unsigned char attrib = STD_ATTR);

	// SETPOS: moves the CGA hardware cursor to x/y.
	void setpos(int x, int y);

	// GETPOS: reads the current cursor position back from the CRT
	//         controller.
	void getpos(int &x, int &y) const;

	// PRINT: writes length characters starting at the current cursor
	//        position, interpreting '\n', wrapping at the end of a line
	//        and scrolling at the bottom of the screen.
	void print(char *text, int length, unsigned char attrib = STD_ATTR);

	// CLEAR: blanks the whole screen and homes the cursor.
	void clear(unsigned char attrib = STD_ATTR);

private:
	// The video memory is mapped into the *memory* address space and is
	// therefore reachable through an ordinary pointer.  Two bytes per
	// character cell: even address = ASCII code, odd address = attribute.
	static char * const CGA_START;

	// The CRT controller's registers live in the *I/O* address space.
	// There are far more of them than there are I/O ports available, so
	// they are multiplexed: write the register number to the index port,
	// then read or write that register's value through the data port.
	const IO_Port index_port; // 0x3d4
	const IO_Port data_port;  // 0x3d5

	// CRTC register numbers holding the two halves of the cursor offset
	enum { CURSOR_HIGH = 14, CURSOR_LOW = 15 };

	// SCROLLUP: moves rows 1..ROWS-1 one line up and blanks the last row.
	void scrollup(unsigned char attrib);
};

#endif
