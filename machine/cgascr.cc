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
	if(x<0 || x>=COLUMNS|| y<0 || y>=ROWS){
		return;
	}
	char* pos = CGA_START +2 * (y*COLUMNS+x);
	*pos = c;
	*(pos +1)=attrib;
}

void CGA_Screen::setpos(int x, int y)
{
	if( x<0 || x>=COLUMNS || y<0|| y>=ROWS)
		return;
	
	int offset = y * COLUMNS + x;

	index_port.outb(CURSOR_HIGH);
	data_port.outb(offset >> 8);
	index_port.outb(CURSOR_LOW);
	data_port.outb(offset & 0xff);
}

void CGA_Screen::getpos(int &x, int &y) const
{
	index_port.outb(CURSOR_HIGH);
	int offset = data_port.inb() << 8;
	index_port.outb(CURSOR_LOW);
	offset |= data_port.inb();

	x = offset % COLUMNS;
	y = offset /COLUMNS;

}

void CGA_Screen::print(char *text, int length, unsigned char attrib)
{
	int x,y;
	getpos(x,y);

	for( int i=0; i< length;i++){
		if(text[i] == '\n'){
			x=0;
			y++;
		}
		else{
			show(x,y, text[i],attrib);
			x++;
		}
		if(x>=COLUMNS){
			x=0;
			y++;
		}
		if(y>=ROWS){
			scrollup(attrib);
			y = ROWS -1;
		}

		setpos(x,y);

	}
}

void CGA_Screen::clear(unsigned char attrib)
{
	char* pos = CGA_START;
	char* const end = CGA_START + 2* COLUMNS *ROWS;

	while(pos < end){
		*pos++ = ' ';
		*pos++=attrib;
	}

	setpos(0,0);

}

void CGA_Screen::scrollup(unsigned char attrib)
{
	char* dst = CGA_START;
	char* src = CGA_START +2* COLUMNS;
	char* const end = CGA_START + 2* COLUMNS * ROWS;

	while(src<end){
		*dst++ = *src++;
	}

	while(dst < end){
		*dst++ = ' ';
		*dst++= attrib;
	}

}