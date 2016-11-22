// Static variables
#define lcd_ticrate 20

// LCD stuff
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// This holds the music string in bytes
byte mus_incomingByte;

// The Bytes are then stored into this temp array
char mus_temp[128];

// We place whatever is in temp+32 -> end, into this array and send it.
char mus_toSend[128];

// char arrays to hold what to print on different y levels of the lcd
char lcd_bot[16]; //bottom
char lcd_top[16]; //top

// Update rate / fps
int lcd_updaterate;

void setup() 
{ 
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.print("Welcome to test");
  lcd_updaterate = 1000 / lcd_ticrate; // Currently lcd_ticrate is set to 20, thus this will run 20 times a second.
  delay(1000);
}



void loop() 
{  
  // if there is bytes available coming from the serial port
  if (Serial.available()) 
  {
    // Read in music string
    Serial.readBytes(mus_temp, 96);

    // Update top and bottom arrays for lcd
    for(int i = 0; i < 16 ; i++)
      lcd_top[i] = mus_temp[i];

    // Bottom
    for(int i = 16; i < 32 ; i++)
      lcd_bot[i - 16] = mus_temp[i];

    // Display it on the lcd now.
    // First we clear.
    lcd.clear();

    //  We print the top
    lcd.setCursor(0, 0);
    lcd.print(lcd_top);

    // We print the bottom
    lcd.setCursor(0, 1);
    lcd.print(lcd_bot);

    // Done displaying to LCD.
    // Transfer the next 32 bits from temp into toSend.
    for(int i = 32; i < 128; i++)
      mus_toSend[i - 32] = mus_temp[i];

    // Send the newly transferred stuff.
    Serial.print(mus_toSend);
  }

  
  
  delay(lcd_updaterate); 
}
