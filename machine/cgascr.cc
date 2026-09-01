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

#include "machine/cgascr.h"

/* Add your code here */

char *const CGA_Screen::CGA_START = (char *)0xb8000;


CGA_Screen::CGA_Screen():index_port(0x3d4),data_port(0x3d5)
{
	clear();
}

void CGA_Screen::show(int x, int y, char c, unsigned char attrib)
{
	if(x<0 || x<=COLUMNS|| y<0 || y>=ROWS){
		return;
	}
	char* pos = CGA_START +2 * (y*COLUMNS+x);
	*pos = c;
	*(pos +1)=attrib;

}

void CGA_Screen::setpos(int x, int y)
{
}

void CGA_Screen::getpos(int &x, int &y) const
{
}

void CGA_Screen::print(char *text, int length, unsigned char attrib)
{
}

void CGA_Screen::clear(unsigned char attrib)
{

}