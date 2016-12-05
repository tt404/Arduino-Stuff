#include <LiquidCrystal.h>
#define lcd_ticrate 20

LiquidCrystal lcd(12,11,5,4,3,2);

// This holds the music string in bytes
byte mus_incomingByte;

// The Bytes are then stored into this temp array
char mus_temp[128];

// char arrays to hold what to print on different y levels of the lcd
int state = 0;
char lcd_bot[16]; //bottom
char lcd_top[16]; //top

// Update rate / fps
int lcd_updaterate;

// For debugging
int debug = false;

void setup() 
{
  // Enable the lcd
  lcd.begin(16,2);

  // Begin communication
  Serial.begin(9600);

  // Welcome message
  lcd.setCursor(0, 0);
  lcd.print("Welcome to test");

  // Set the frame rate (see lcd_ticrate)
  lcd_updaterate = 1000 / lcd_ticrate; // Currently lcd_ticrate is set to 20, thus this will run 20 times a second.

  // Wait.
  delay(1000);
}

void loop() 
{  

  // This is the val for the display "mode"
  Serial.print(mus_temp[0]);
  
  // If there is bytes available coming from the serial port
  if (Serial.available()) 
  {
    // Read in music string
    Serial.readBytes(mus_temp, 128);

    // Send the stuff.
    Serial.print(mus_temp + 33);
    

    if (state != mus_temp[0])
    {
      switch (mus_temp[0]) 
      {
        case 1:
          Table1();
          break;
        case 2:
          Table2();
          break;
        default:
          break;
      }
      state = mus_temp[0];
    }
  
    // Update top and bottom arrays for lcd
    for(int i = 0; i < 16 ; i++)
    {
      if(mus_temp[i+1] == 1)
        lcd_top[i] = 0xA0;
      else
        lcd_top[i] = mus_temp[i+1] - 2;
    }
    
    // Bottom
    for(int i = 16; i < 32 ; i++)
    {
      if(mus_temp[i+1] == 1)
        lcd_bot[i - 16] = 0xA0;
      else
        lcd_bot[i - 16] = mus_temp[i+1] - 2;
    }

    // Display it on the lcd now.
    // First we clear.
    lcd.clear();

    //  We print the top
    lcd.setCursor(0, 0);

    // Write each character individually for top array.
    for(int i = 0; i < 16; i++)
    lcd.write(byte(lcd_top[i]));

    // We print the bottom
    lcd.setCursor(0, 1);

    // Write each character individually for bottom array.
    for(int i = 0; i < 16; i++)
    lcd.write(byte(lcd_bot[i]));
  }
  
  delay(lcd_updaterate); 
}

void Table1()
{
  unsigned char Ox1[8][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F},
    {0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F},
    {0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F},
    {0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F},
    {0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F},
    {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F}    
  };
  
  for(byte i = 0; i < 8; ++i)
    lcd.createChar(i, Ox1[i]);
}

void Table2()
{
  unsigned char Ox2[8][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}    
  };
  for (byte i = 0; i < 8; ++i)
    lcd.createChar(i, Ox2[i]);
  
}
