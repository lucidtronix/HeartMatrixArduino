/* LucidTronix Heart Matrix.
 * A volume visualizwer using the Heart Matrix's
 * on board LM386 amplifier chip and the electret microphone
 * See the tutorial at: 
 * http://lucidtronix.com/tutorials/35 */
 
#include <MsTimer2.h>
#include <HeartMatrix.h>

// dataPin is on 5, Latch is on 6, and clock is on 7
HeartMatrix hm = HeartMatrix(5,6,7); 

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
   for (int i = 0 ; i < buffer_size; i++) noises[i] = 512;
   hm.animate();
}

void loop() {
  hm.on();
  sound_display();
}

void displayer2(){  
  hm.displayer();
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
