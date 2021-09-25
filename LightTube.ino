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

#define NUMPIXELS 58 // Number of LEDs in strip

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

uint32_t  RED;
uint32_t  YELLOW;
uint32_t  ORANGE;
uint32_t  GREEN;
uint32_t  TEAL;
uint32_t  CYAN;
uint32_t  BLUE;
uint32_t  PURPLE;
uint32_t  MAGENTA;
uint32_t  WHITE;
uint32_t  OFF;
uint32_t FILL_DELAY = 0;
uint32_t colors[8];

void setup()
{
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
  pinMode(AUDIO_SENSE_PIN, INPUT);

  RED = strip.Color(0, 255, 0);
  YELLOW = strip.Color(0, 255, 150);
  ORANGE = strip.Color(0, 255, 40);
  GREEN = strip.Color(0,0, 255);
  TEAL = strip.Color(120, 0, 255);
  CYAN = strip.Color(255,0, 255);
  BLUE = strip.Color(255,0, 0);
  PURPLE = strip.Color(255, 180, 0);
  MAGENTA = strip.Color(20, 255, 0);
  OFF = strip.Color(0,0,0);

  colors[0] = RED;
  colors[1] = ORANGE;
  colors[2] = YELLOW;
  colors[3] = GREEN;
  colors[4] = BLUE;
  colors[5] = PURPLE;
  colors[6] = MAGENTA;
  colors[7] = OFF;



  //colors[] = {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, MAGENTA, OFF};

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

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}


void ledAnimate()
{

  for (int i=0; i<8; i++)
  {
    for (int j=0; j<(NUMPIXELS + 10); j++)
    {
    strip.setPixelColor(head, colors[i]);
    strip.setPixelColor(tail, 0);
    strip.show();
    delay(1);
    head++;
    tail++;
    }
    //if (++tail >= NUMPIXELS)
    //{
      head = 0;
      tail = -10;
      //continue;
    //}
    //}
  }

    // for(int i=0; i<255; i++) // 159
    // {
    //   strip.setPixelColor(head, color); // 'On' pixel at head
    //   strip.setPixelColor(tail, 0);     // 'Off' pixel at tail
    //   strip.show();                     // Refresh strip
    //   delay(20);                        // Pause 20 milliseconds (~50 FPS)

    //   if (++head >= NUMPIXELS)
    //   {                           // Increment head index.  Off end of strip?
    //       head = 0;               //  Yes, reset head index to start
    //       if ((color >>= 4) == 0) //  Next color (R->G->B) ... past blue now?
    //           color = 0xFF0000;   //   Yes, reset to red
    //   }
    //   if (++tail >= NUMPIXELS)
    //   {
    //     tail = 0; // Increment, reset tail index
    //   }
          

    // }
    // strip.clear();
    // strip.show();

  Serial.println(F("Done Animating LEDs"));
}

void loop()
{
    int audio_reading = analogRead(AUDIO_SENSE_PIN);
    //Serial.println(audio_reading);
    if (audio_reading > 20)
    {
        Serial.println(F("Playing track 002"));
        musicPlayer.startPlayingFile("/track002.mp3");
        //rainbow(20);
        // theaterChase(strip.Color(255,0,0),10);
        // theaterChase(strip.Color(0,255,0),10);
        // theaterChase(strip.Color(0,0,255),10);
        ledAnimate();
    }
}