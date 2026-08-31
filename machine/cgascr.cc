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

CGA_Screen::CGA_Screen():x(0),y(0){}


void CGA_Screen::show (int x, int y, char c, unsigned char attrib){
	char* pos;
	pos = CGA_START + 2*(80*y+x);
	*pos = c;
	*(pos+1) = attrib;	
	}


void CGA_Screen::setpos (int x, int y){
	this->x=x;
	this->y = y;
	//pos = CGA_START + 2*(80*y+x);
}


void CGA_Screen::getpos (int &x, int &y){
	x=this->x;
	y=this->y;

}


void CGA_Screen::print (char* text, int length, unsigned char attrib)
{
	for(int  i=0; i<length; i++){
		show(this->x,this->y,text[i],attrib);
		this->x=this->x+1;
	}
	
}
