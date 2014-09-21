/* LucidTronix Heart Matrix.
 * A clock using the heart matrix's 
 * on board MCP79410 real time clock chip
 * See the tutorial at: 
 * http://lucidtronix.com/tutorials/36 */
 
#include <Wire.h>
#include <MsTimer2.h>
#include <HeartMatrix.h>
#include <MCP79410.h>

String stime = "";
unsigned long last_update = 0;
boolean show_24_hours = false;
HeartMatrix hm = HeartMatrix(5,6,7); // dataPin is on 5, Latch is on 6, and clock is on 7
MCP79410 clock = MCP79410();

void displayer2(){  
  hm.displayer();
}

void setup()
{
   Wire.begin(); 
   //clock.setDateTime(__DATE__,__TIME__);
   MsTimer2::set(1,displayer2); 
   MsTimer2::start(); 
   pinMode(10, INPUT);
}
 
void loop(){
 hm.on();
 hm.set_scroll_wait(max(30, analogRead(0)/6)); 

 if (millis() - last_update > 1000){
    last_update = millis();
    stime = "  ";    
    byte a_hour = clock.hour24();
    String a_hour_s = String(a_hour);
    int hour_int = a_hour_s.toInt();
    String ampm = "am";
    if (!show_24_hours && hour_int > 12){
      ampm = "pm";
      hour_int -= 12;
      a_hour_s = String(hour_int);
    }
    if (a_hour == 0) stime += String(12); 
    else stime += a_hour_s;  
    stime += ":";
    byte a_min = clock.minute();
    if (a_min < 10) stime += String(0);
    stime += String(a_min);
    stime += ":";
    byte a_sec = clock.second();
    if (a_sec < 10) stime += String(0);
    stime += String(a_sec);
    if(!show_24_hours) stime += ampm;
    hm.set_message(stime);
  }
  
  if(digitalRead(10) == HIGH && millis() - last_update > 500){
    last_update = millis();
    show_24_hours = !show_24_hours;
  }
}
