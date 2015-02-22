/* LucidTronix Heart Matrix.
 * A clock using the heart matrix's 
 * on board MCP79410 real time clock chip
 * See the tutorial at: 
 * http://lucidtronix.com/tutorials/36 */
 
#include <Wire.h>
#include <MsTimer2.h>
#include <HeartMatrix.h>
#include <MCP79410.h>

HeartMatrix hm = HeartMatrix(5,6,7); // dataPin is on 5, Latch is on 6, and clock is on 7

void displayer2(){  
  hm.displayer();
}

int dataPin = 1;
int clockPin = 0;
int latchPin = A2;

// use this option for OSX. 
// Comment it out if using Windows or Linux:
char ctrl_key = KEY_LEFT_GUI;
// use this option for Windows and Linux.
// leave commented out if using OSX:
//  char ctrl_key = KEY_LEFT_CTRL; 

const int buffer_size = 32;
String buffer = " ";
int char_index = 0;
boolean shifting = false;
boolean controlling = false;
unsigned long last_key_press = 0;

char* letters[] ={"yz EDS.C", "qrstuvwx","ijklmnop","abcdefgh" };

int num_registers = 4;
byte button_states[] = {0x00,0x00,0x00,0x00};  //01001000



void setup() {
  Serial.begin(9600);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT); 
  pinMode(dataPin, INPUT);
  MsTimer2::set(1,displayer2); 
  MsTimer2::start(); 
  pinMode(10, INPUT);
}

void loop() {
 if (millis() - last_key_press > 300) read_keyboard();
 hm.on();
 hm.set_scroll_wait(max(30, analogRead(0)/6)); 
}

void read_keyboard(){
  digitalWrite(latchPin,0);
  for (int i = 0; i < num_registers; i++){
    button_states[i] = shiftIn(dataPin, clockPin);
  }
  digitalWrite(latchPin,1);
  boolean something_in = false;
  for (int i = 0; i < num_registers; i++){    
    something_in = translate_buttons_2_keys(button_states[i], letters[i]);
    if (something_in) last_key_press = millis(); 
  }
}

boolean translate_buttons_2_keys(byte buttons, char* keys){
  for(int i = 0 ; i < 8; i++){
     if ( (1 << i) & buttons ){
       // Check for control characters
       if(keys[i] == 'E') {
         Keyboard.write(KEY_RETURN);
       }
       else if(keys[i] == 'D') {
         buffer = buffer.substring(0, buffer.length()-1);
         char_index--;
         Keyboard.write(KEY_BACKSPACE);
       }
       else if(keys[i] == 'S'){
         shifting = !shifting;
         if(shifting) Keyboard.press(KEY_LEFT_SHIFT);
         else Keyboard.release(KEY_LEFT_SHIFT);
       }
       else if(keys[i] == 'C') {
         buffer = " " ;
         char_index = 0;
         controlling = !controlling;
         if(controlling) Keyboard.press(ctrl_key);
         else Keyboard.release(ctrl_key);
       }
       else {
         if(buffer.length() < char_index + 1) buffer += String(" ");
         if(shifting) buffer[char_index] = toupper(keys[i]);
         else buffer[char_index] = keys[i];
         char_index++;
         if(char_index == buffer_size) char_index = 0;
         Keyboard.print(keys[i]);
       }
       hm.set_message(buffer);

       return true;
     }     
  }
 return false; 
}

byte shiftIn(int myDataPin, int myClockPin) { 
  int i;
  int temp = 0;
  int pinState;
  byte myDataIn = 0;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, INPUT);
  for (i=7; i>=0; i--)
  {
    digitalWrite(myClockPin, 0);
    delayMicroseconds(20);
    temp = digitalRead(myDataPin);
    if (temp) {
      pinState = 1;
      myDataIn = myDataIn | (1 << i);
    }
    else {
      pinState = 0;
    }
    digitalWrite(myClockPin, 1);
  }
  return myDataIn;
}
