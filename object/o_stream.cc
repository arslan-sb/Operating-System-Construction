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

#include "object/o_stream.h"

/* Add your code here */ 


O_Stream::O_Stream():base(10){}

O_Stream& O_Stream::operator<<(char c)
{
    put(c);
    return *this;
}

O_Stream& O_Stream::operator<<(unsigned char c)
{
    return *this << (char) c;

}

O_Stream& O_Stream::operator<<(const char* string)
{
    while(*string)
    {
        put(*string++);
    }
    return *this;
}

O_Stream& O_Stream::operator<<(bool b)
{
    return *this << (b ? "true": "false"); 
}


void O_Stream::put_unsigned(unsigned long long value)
{
    char digits[64];
    int n= 0;

    do{
        unsigned int d = (unsigned int )( value % (unsigned)(base));
        digits[n++]= (d<10)? ('0'+d) : ('a'+d-10);
        value /= (unsigned)base;  
    }while(value !=0);

    while(n>0){
        put(digits[--n]);
    } 
}

void O_Stream::put_signed(long long value)
{
    if(value<0){
        put('-');
        put_unsigned(~(unsigned long long)value +1);
    }
    else{
        put_unsigned((unsigned long long)value);
    }
}


O_Stream& O_Stream::operator<<(void *pointer)
{
    int old_base= base;
    base = 16;
    *this<< "0x";

    put_unsigned((unsigned long long) pointer);
    base =old_base;

    return *this;
}

O_Stream& O_Stream::operator<<(int value)
{
    if(base ==10)
    {
        put_signed((long long) value);
    }
    else{
        put_unsigned((unsigned int ) value);
    }
    return *this;
}


O_Stream& O_Stream::operator<<(unsigned int value)
{
    put_unsigned((unsigned long )value);
    return *this;
}



O_Stream& O_Stream::operator<<(short value)
{
    if(base == 10){
        put_signed((long long) value);
    }
    else{
        put_unsigned((unsigned short) value);
    }

    return *this;
}

O_Stream& O_Stream::operator<<( unsigned short value)
{
    put_unsigned(value);

    return *this;
}


O_Stream& O_Stream::operator<<(long value)
{
    if(base == 10)
    {
        put_signed((long long) value);
    }
    else{
        put_unsigned((unsigned long) value);
    }


    return *this;
}

O_Stream& O_Stream::operator<<(unsigned long value)
{
   
        put_unsigned((unsigned long long) value);


    return *this;
}

O_Stream& O_Stream::operator<<(long long value)
{
    if( base == 10)
    {
        put_signed(value);
    }
    else{
        put_unsigned((unsigned long long) value);
    }

    return *this;
}

O_Stream& O_Stream::operator<<(unsigned long long value)
{
    
        put_unsigned(value);


    return *this;
}



O_Stream& O_Stream::operator<<(O_Stream& (*fkt)(O_Stream&))
{
    return fkt(*this);
}

O_Stream& endl(O_Stream& os)
{
    os<<"\n";
    os.flush();

    return os;
}

O_Stream& bin(O_Stream& os){ os.base = 2; return os;}
O_Stream& oct(O_Stream& os){ os.base = 8; return os;}
O_Stream& dec(O_Stream& os){ os.base = 10; return os;}
O_Stream& hex(O_Stream& os){ os.base = 16; return os;}

