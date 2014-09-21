/* LucidTronix Heart Matrix LEO LED Display.
 * Text on the Heart Matrix
 * For instructions details and schematic, See:
 * http://www.lucidtronix.com/tutorials/57  */
#include <MsTimer2.h>
#include <HeartMatrix.h>

HeartMatrix hm = HeartMatrix(5,6,7); // dataPin is on 5, Latch is on 6, and clock is on 7

void displayer2(){  
  hm.displayer();
}

void setup()
{
   MsTimer2::set(1,displayer2); 
   MsTimer2::start(); 
   hm.set_message(" Hello I'm the Heart Matrix from LucidTroniX! ");
}
   
void loop(){
  hm.on();
  hm.set_scroll_wait(max(20, analogRead(0)/4)); 
}
