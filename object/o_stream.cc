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


// O_Stream& O_Stream::operator<<(short value)
// {
//     put((char) value);

//     return *this;
// }