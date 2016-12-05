#pragma GCC optimize ("-O1")
#define FHT_N 128
#define LOG_OUT 1
#define LIN_OUT8 0
#define LIN_OUT 0
#define SCALE 1

int fht_max;
int fht_min;
const int ARRSIZE = 48;
const int auxPin0 = 0;
int sampleArr[ARRSIZE];
int lvls[48];
char bars[100];

const int btnMode = 3;
const int btnMirror = 4;
const int ledMode = 5;
const int ledMirror = 6;
int curMode = 0;
int prvMode = 0;
int curMirror = 0;
int prvMirror = 0;

#include <FHT.h>

void setup() {

  // FFT stuff
  //TIMSK0 = 0; // turn off timer0 for lower jitter - delay() and millis() killed
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
  
  Serial.begin(9600);  
  pinMode(auxPin0, INPUT);
  pinMode(btnMode, INPUT);
  pinMode(ledMode, OUTPUT);
  pinMode(btnMirror, INPUT);
  pinMode(ledMirror, OUTPUT);
  bars[99] = '/0';

  // Use Analog reads
  myFHT();
  normalizeValues();
}

void loop() {
  

  // Mode Button:
  // 1: bars growing up
  // 2: bars growing from the center
  
  int nowMode = digitalRead(btnMode);
  if (nowMode == HIGH && prvMode == LOW)
  {
    curMode = (curMode + 1) & 0x1;
  } 
  prvMode = nowMode;

  // Mirror Button:
  // Left to Right i.e. Lows -> Med -> High
  // Right to Left i.e. High -> Med -> Low

  int nowMirror = digitalRead(btnMirror);
  if (nowMirror == HIGH && prvMirror == LOW)
  {
    curMirror = (curMirror + 1) & 0x1;
  }
  
  prvMirror = nowMirror;
  

  switch ((curMode << 1) ^ curMirror)
  {
    case 0: // Mode = 0, Mirror = 0
      lvlsToBars0();
      //Serial.print(0);
      break;
      
    case 1: // Mode = 0, Mirror = 1
      lvlsToBars1();
      //Serial.print(1);
      break;
    case 2: // Mode = 1, Mirror = 0
      lvlsToBars2();
      //Serial.print(2);
      break;
    case 3: // Mode = 1, Mirror = 1
      lvlsToBars3();
      //Serial.print(3);
      break;
      
    default:
      break;
  }
  
  Serial.print(bars);
  digitalWrite(ledMode, curMode);
  digitalWrite(ledMirror, curMirror);


  // Use Analog reads
  myFHT();
  normalizeValues();
  delay(2000);
}

void myFHT()
{

  //INPUT: sampleARR
  //OUTPUT: lvls[48] with values [0..16]

    cli();
    for (int i = 0; i < FHT_N; ++i) {
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      //Serial.print(m + "\n");
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fht_input[i] = k; // put real data into even bins
    }
    
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fft
    fht_run(); // process the data in the fft
    fht_mag_log(); // take the output of the fft
    sei(); // turn interrupts back on

  // fft_log_out (has 64 integers)
  fht_max = 0;
  fht_min = 10000;
  for (int i = 8; i < 56; ++i) {
    if (fht_log_out[i] > fht_max) {
      fht_max = fht_log_out[i];
    }
    if (fht_log_out[i] < fht_min) {
      fht_min = fht_log_out[i];
    }

    lvls[i-8] = fht_log_out[i];
  }
  
}

void normalizeValues()
{
  for(int i = 0; i < 48; ++i)
  {
    lvls[i] = map(lvls[i],fht_min, fht_max, 0, 16);
  }
}


void lvlsToBars0()
{
  bars[99] = '\0';
  bars[0] = curMode + 1;
  bars[33] = curMode + 1;
  bars[66] = curMode + 1;
  for(int i = 0; i < 48; ++i)
  {
    //Serial.print(mapTop(i)); Serial.print(","); Serial.print(mapBot(i)); Serial.write(lvls[i]+'0'); Serial.print("|"); 
    if (lvls[i] > 8)
    {
      bars[mapTop(i)] = (lvls[i] - 8) + 1;
      bars[mapBot(i)] = 9;
    }
    else
    {
      bars[mapTop(i)] = 1;
      bars[mapBot(i)] = lvls[i] + 1;
    }
  }
  //delay(5000);
}
void lvlsToBars1()
{
  bars[99] = '\0';
  bars[0] = curMode + 1;
  bars[33] = curMode + 1;
  bars[66] = curMode + 1;
  for(int i = 0; i < 48; ++i)
  {
    if (lvls[i] > 8)
    {
      bars[mapTop(47-i)] = (lvls[i] - 8) + 1;
      bars[mapBot(47-i)] = 9;
    }
    else
    {
      bars[mapTop(47-i)] = 1;
      bars[mapBot(47-i)] = lvls[i] + 1;
    }
  }
}

void lvlsToBars2()
{
  bars[99] = '\0';
  bars[0] = curMode + 1;
  bars[33] = curMode + 1;
  bars[66] = curMode + 1;
  for(int i = 0; i < 48; ++i)
  {
    lvls[i] = lvls[i] / 2;

    bars[mapTop(i)] = lvls[i] + 1;
    bars[mapBot(i)] = ( 8 - lvls[i] ) % 8 + 1;
  }
}

void lvlsToBars3()
{
  bars[99] = '\0';
  bars[0] = curMode + 1;
  bars[33] = curMode + 1;
  bars[66] = curMode + 1;
  for(int i = 0; i < 48; ++i)
  {
    lvls[i] = lvls[i] / 2;
    
    bars[mapTop(47-i)] = lvls[i] + 1;
    bars[mapBot(47-i)] = ( 8 - lvls[i] ) % 8 + 1;
  }
}

int mapTop(int i)
{
  return (i+(i / 16)*16)+(i/16+1);
}

int mapBot(int i)
{
  return (i+(i / 16)*16)+16+(i/16+1);
}

