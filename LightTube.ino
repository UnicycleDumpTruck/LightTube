// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// define the pins used
//#define CLK 13       // SPI Clock, shared with SD card
//#define MISO 12      // Input data, from VS1053/SD card
//#define MOSI 11      // Output data, to VS1053/SD card
// Connect CLK, MISO and MOSI to hardware SPI pins. 
// See http://arduino.cc/en/Reference/SPI "Connections"

// These are the pins used for the breakout example
//#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
//#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
//#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
// #define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
// #define SHIELD_CS     7      // VS1053 chip select pin (output)
// #define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     6      // VS1053 chip select pin (output)
#define SHIELD_DCS    10      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
// #define CARDCS 4     // Card chip select pin
#define CARDCS 5     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
// #define DREQ 3       // VS1053 Data request, ideally an Interrupt pin
#define DREQ 9       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create breakout-example object!
  //Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
  // create shield-example object!
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);


/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}









// Simple strand test for Adafruit Dot Star RGB LED strip.
// This is a basic diagnostic tool, NOT a graphics demo...helps confirm
// correct wiring and tests each pixel's ability to display red, green
// and blue and to forward data down the line.  By limiting the number
// and color of LEDs, it's reasonably safe to power a couple meters off
// the Arduino's 5V pin.  DON'T try that with other code!

#include <Adafruit_DotStar.h>
//#include <SPI.h> // included above

#define NUMPIXELS 30 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN A1
#define CLOCKPIN A2
Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
// The last parameter is optional -- this is the color data order of the
// DotStar strip, which has changed over time in different production runs.
// Your code just uses R,G,B colors, the library then reassigns as needed.
// Default is DOTSTAR_BRG, so change this if you have an earlier strip.

// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
//Adafruit_DotStar strip(NUMPIXELS, DOTSTAR_BRG);


// Audio Sense Variables
#define AUDIO_SENSE_PIN A0


void setup()
{
    strip.begin(); // Initialize pins for output
    strip.show();  // Turn all LEDs off ASAP
    pinMode(AUDIO_SENSE_PIN, INPUT);

    // Audio Setup:
    Serial.begin(9600);
    // while (!Serial) {
    //     ; // wait for serial port to connect. Needed for native USB port only
    // }
    Serial.println("LightTube, Adafruit VS1053 Simple Test");

    if (! musicPlayer.begin()) { // initialise the music player
        Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
        while (1);
    }
    Serial.println(F("VS1053 found"));
    
    if (!SD.begin(CARDCS)) {
        Serial.println(F("SD failed, or not present"));
        while (1);  // don't do anything more
    }

    // list files
    printDirectory(SD.open("/"), 0);
    
    // Set volume for left, right channels. lower numbers == louder volume!
    musicPlayer.setVolume(20,20);

    // Timer interrupts are not suggested, better to use DREQ interrupt!
    //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

    // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
    // audio playing
    musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
    
    // Play one file, don't return until complete
    //Serial.println(F("Playing track 001"));
    //musicPlayer.playFullFile("/track001.mp3");
    // Play another file in the background, REQUIRES interrupts!
}

// Runs 10 LEDs at a time along strip, cycling through red, green and blue.
// This requires about 200 mA for all the 'on' pixels + 1 mA per 'off' pixel.

int head = 0, tail = -10;  // Index of first 'on' and 'off' pixels
uint32_t color = 0xFF0000; // 'On' color (starts red)

void ledAnimate()
{
    Serial.println(F("Playing track 002"));
    musicPlayer.startPlayingFile("/track002.mp3");

    for(int i=0; i<159; i++)
    {
        strip.setPixelColor(head, color); // 'On' pixel at head
        strip.setPixelColor(tail, 0);     // 'Off' pixel at tail
        strip.show();                     // Refresh strip
        delay(20);                        // Pause 20 milliseconds (~50 FPS)

        if (++head >= NUMPIXELS)
        {                           // Increment head index.  Off end of strip?
            head = 0;               //  Yes, reset head index to start
            if ((color >>= 8) == 0) //  Next color (R->G->B) ... past blue now?
                color = 0xFF0000;   //   Yes, reset to red
        }
        if (++tail >= NUMPIXELS)
            tail = 0; // Increment, reset tail index

        }
  Serial.println(F("Done Animating LEDs"));
}

void loop()
{
    int audio_reading = analogRead(AUDIO_SENSE_PIN);
    //Serial.println(audio_reading);
    if (audio_reading > 20)
    {
        ledAnimate();
    }
}