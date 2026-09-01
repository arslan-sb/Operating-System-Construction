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

#include "io_port.h"

class CGA_Screen
{

public:
	CGA_Screen(const CGA_Screen &copy) = delete;		// prevent copying
	CGA_Screen &operator=(const CGA_Screen &) = delete; // prevent assignment

	enum { COLUMNS =80, ROWS =25};
	enum { STD_ATTR = 0X0f};



	CGA_Screen();
	/* Add your code here */
	void show(int x, int y, char c, unsigned char attrib=STD_ATTR);
	void setpos(int x, int y);
	void getpos(int &x, int &y) const;
	void print(char *text, int length, unsigned char attrib=STD_ATTR);
	void clear( unsigned char attrib =STD_ATTR);

private:
	/* Add your code here */
	static char* const CGA_START; // 0xb8000
	const IO_Port index_port;	  // 0x3d4
	const IO_Port data_port;	  // 0x3d5
	enum{ CURSOR_HIGH = 14, CURSOR_LOW = 15 };
	
	void scrollup(unsigned char attrib);
	/* Add your code here */
};

/* Add your code here */

#endif