/* LucidTronix Heart Matrix LEO LED Display.
 * Demoing the heart matrix clock and sound sensor
 * For instructions details and schematic, See:
 * http://www.lucidtronix.com/tutorials/57
 */
#include <Wire.h>
#include <MsTimer2.h>
#include <HeartMatrix.h>
#include <MCP79410.h>

// dataPin is on 5, Latch is on 6, and clock is on 7
HeartMatrix hm = HeartMatrix(5,6,7);

MCP79410 clock = MCP79410();

int mode = 2;
int num_modes = 3;
unsigned long last_press = 0;

// clock global variables
String stime = "";
unsigned int last_update = 0;

// sound global variables
const int buffer_size = 15;
int noises[buffer_size];
const int num_cols = 10;
int volumes[num_cols];

int cur_index = 0;
unsigned long last_shift = 0;
int mic_pin = 1;
int delay_speed = 40;
unsigned long max_var = 0;
unsigned long min_var = 99999999;

void setup() {
   Serial.begin(9600);
   MsTimer2::set(1,displayer2);
   MsTimer2::start();
   for (int i = 0 ; i < num_cols; i++) volumes[i] = 8;
   for (int i = 0 ; i < buffer_size; i++) noises[i] = random(300,612);
   if(mode == 2) hm.set_message(" Hello I'm the Heart Matrix! ");
   if(mode == 0) hm.animate();
   
   Wire.begin(); 
   //clock.setDateTime(__DATE__,__TIME__);
}
  
void loop() {
  if(mode == 0) sound_display();
  else if(mode == 1) clock_display();
  else if(mode == 2) text_display();
  check_buttons();
}

void displayer2(){
  hm.displayer();
}

void check_buttons(){
 if(digitalRead(9) == HIGH && millis() - last_press > 250){
   mode = ++mode % num_modes;
   if(mode == 0) hm.animate();
   if(mode == 2) hm.set_message(" Hello I'm the Heart Matrix! "); 
   last_press = millis();
 }
 hm.set_scroll_wait(max(20, analogRead(0)/4)); 
}

void text_display(){
  hm.on();  
}
void clock_display(){
  hm.on();  
  if (millis() - last_update > 1000){
    stime = " ";
    
    byte a_hour = clock.hour24();
    String a_hour_s = String(a_hour,HEX);
    int hour_int = a_hour_s.toInt();
    String ampm = "am  ";
    if (hour_int >= 12){
      ampm = "pm  ";
      hour_int -= 12;
      a_hour_s = String(hour_int);
    }
    if (a_hour == 0 || a_hour == 12 ) stime += String(12); 
    else stime += a_hour_s;  
    stime += ":";
    byte a_min = clock.minute();
    if (a_min < 10 ) stime += String(0);
    stime += String(a_min,HEX);
    stime += ":";
    byte a_sec = clock.second();
    if (a_sec < 10 ) stime += String(0);
    stime += String(a_sec,HEX);
    hm.set_message(stime+ampm );
    last_update = millis();
  }
}

void sound_display(){
   // Get a value from the electret microphone   
   noises[cur_index] = analogRead(mic_pin) ;
   cur_index++;
   if(cur_index == buffer_size) cur_index = 0;
   // Control the scroll speed with a potentiometer on analog pin 0
   delay_speed = max(10, (analogRead(0)/4));
   
   // Shift the waveform over one column
   if (millis() - last_shift > delay_speed){
     last_shift = millis();
     unsigned int avg = average(noises, buffer_size);
     unsigned long var = variance(noises, buffer_size);
     max_var = max(var, max_var);
     min_var = min(var, min_var);
     var = map(var,min_var , max_var, 0, 9);
     for (int i = 1 ; i < num_cols; i++){
      volumes[i-1] = volumes[i];
     }
     volumes[num_cols-1] = var;
   }
   
   // Write the wave form to LED display
   for (int i = 0 ; i < num_cols; i++){
     for (int j = 0 ; j < volumes[i]; j++){
       hm.set_pixel(i, j, true);  
     }
     for (int j = volumes[i] ; j < 8; j++){
       hm.set_pixel(i, j, false);  
     }
   }  
}

int average(int* array, int length){
  int sum = 0;
  int i;
  for(i = 0; i < length ; i++){
    sum += array[i];
  }
  int avg = sum / length ;
  return avg;
}

unsigned long variance(int* array, int length){
  long sum = 0;
  long avg = average(array, length);
  for(int i = 0; i < length ; i++){
    sum += (array[i] - avg)*(array[i] - avg);
  }
  unsigned long var = sum / length;
  return var;	
}
