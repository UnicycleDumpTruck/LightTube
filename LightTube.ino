#include <SD.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <Adafruit_DotStar.h>

#define SHIELD_RESET  -1    // VS1053 reset pin (unused!)
#define SHIELD_CS     6     // VS1053 chip select pin (output)
#define SHIELD_DCS    10    // VS1053 Data/command select pin (output)
#define CARDCS 5            // Card chip select pin
#define DREQ 9              // VS1053 Data request, ideally an Interrupt pin

#define NUMPIXELS 158 // 1 has 158, 2 has 139 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN A2 // Station 2 is A1, Station 1 is A2
#define CLOCKPIN A1 // Station 2 is A2, Station 1 is A1
Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);

// Audio Sense Variables
#define AUDIO_SENSE_PIN A0

Adafruit_VS1053_FilePlayer musicPlayer = 
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


// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  long firstWipePixelHue = 0; 
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    int pixelHue = firstWipePixelHue - (i * 65536L / strip.numPixels());
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    strip.show();
  }
  

  for(long firstPixelHue = 0; firstPixelHue < 1 * 65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue - (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
      // strip.show();
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}


void colorWipe(long color, int wait)
{
  for (int i=0; i<NUMPIXELS; i++)
  {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}


void setup()
{
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
  
  pinMode(AUDIO_SENSE_PIN, INPUT);

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
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(0,0);
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
}


void loop()
{
    int audio_reading = analogRead(AUDIO_SENSE_PIN);
    //Serial.println(audio_reading);
    //delay(10);
    if (audio_reading > 18)
    {
        Serial.println(F("Playing track 002"));
        //colorWipe(GREEN, 5);
        musicPlayer.startPlayingFile("/track002.mp3");
        rainbow(1);
        colorWipe(strip.Color(0,0,0), 5);
        musicPlayer.stopPlaying();
    }
}