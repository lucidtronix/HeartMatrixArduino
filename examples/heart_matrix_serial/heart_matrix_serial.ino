/* LucidTronix write messages from
 * the serial port on the Heart Matrix.
 * For instructions details and schematic
 * http://www.lucidtronix.com/tutorials/32
 * If debug is true outputs whatever was received 
 * On serial1 to the standard USB serial. 
 */
 
#include <MsTimer2.h>
#include <HeartMatrix.h>

String message = " Hi  ";
bool debug = true;

HeartMatrix hm = HeartMatrix(5,6,7); // dataPin is on 5, Latch is on 6, and clock is on 7

void displayer2(){  
  hm.displayer();
}

void setup()
{
   if(debug) Serial.begin(9600); // initialize Serial
   Serial1.begin(9600);    	 // initialize Serial1
   MsTimer2::set(1,displayer2); 
   MsTimer2::start(); 
   hm.set_message(" Hello I'm the Heart Matrix from LucidTroniX! ");
}

void loop() {
   hm.on();    
   while (Serial1.available() ){
     char c = Serial1.read();
     if (c == '/') message = "";
     else {
       String s = String(c);
       message += s;
       hm.set_message(message);
     }
     if(debug){
       Serial.print("got the letter:");
       Serial.println(c);
     }
  } 
}

