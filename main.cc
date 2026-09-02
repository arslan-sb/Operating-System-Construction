/* Add your code here */ 
#include "machine/cgascr.h"


int main()
{
	CGA_Screen screen;

    // 1. Test clear() with a specific attribute 
    // (e.g., 0x1F: White text on Blue background)
    screen.clear(0x1F);

    // 2. Test show()
    // Print 'X' at the top right corner (assuming 80 columns, 25 rows)
    screen.show(79, 0, 'X', 0x1E); // Yellow on Blue

    // 3. Test setpos() and print()
    screen.setpos(0, 2);
    char msg1[] = "Testing setpos and print...\n";
    screen.print(msg1, sizeof(msg1) - 1, 0x1F);

    // 4. Test getpos()
    int cur_x, cur_y;
    screen.getpos(cur_x, cur_y);
    
    // We can't use printf in a bare-metal OS, so let's mark the fetched position
    // with a special character using show() to see if it's correct.
    // It should place a '*' at the start of row 3 (since we printed a newline above)
    screen.show(cur_x, cur_y, '*', 0x1C); // Light Red on Blue

    // Move cursor past the '*'
    screen.setpos(cur_x + 2, cur_y);

    // 5. Test text wrapping and scrollup()
    // We will print 30 lines of text. Since standard CGA is 25 rows, 
    // this will force the screen to scroll up several times.
    char scroll_msg[] = "Line printing to test scrolling logic. Wrap around should also work here if we type enough characters in one single go without newlines. ";
    
    for (int i = 0; i < 30; i++) {
        screen.print(scroll_msg, sizeof(scroll_msg) - 1, 0x0A); // Light Green on Black
        screen.print((char*)"\n", 1, 0x0A);
    }

    // 6. Final message at the bottom to confirm success
    char success_msg[] = "TEST COMPLETED SUCCESSFULLY!";
    screen.print(success_msg, sizeof(success_msg) - 1, 0x8F); // Blinking White on Black

    // Loop indefinitely to keep the screen open (standard OS dev practice)
    //while(1) {}
 
	return 0;
}
