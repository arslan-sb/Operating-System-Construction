/*****************************************************************************/
/* Operating-System Construction                                             */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                         A P P L I C A T I O N                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* The Application class defines the (only) application for OOStuBS.         */
/*****************************************************************************/

#ifndef __application_include__
#define __application_include__

class Application
 
{

public:
	Application (const Application &copy) = delete; // prevent copying
	Application& operator=(const Application&) = delete; // prevent assignment

	// Declaring the deleted copy constructor above suppresses the implicit
	// default constructor, so it has to be requested explicitly.
	Application() = default;

	// ACTION: the application's main routine.
	void action ();
};

#endif
