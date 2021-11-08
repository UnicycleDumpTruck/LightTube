#include <SD.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <Adafruit_DotStar.h>
#include <Adafruit_SleepyDog.h>


// Radio Includes and variables ----------------------------------

#include <RH_RF69.h>
#include <RHReliableDatagram.h>

#define RF69_FREQ 915.0

// Where to send packets to!
//#define DEST_ADDRESS   1
// change addresses for each client board, any number :)
#define MY_ADDRESS     90

// Feather M0 w/Radio
#define RFM69_CS      A4
#define RFM69_INT     A3
#define RFM69_RST     11

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);

int16_t packetnum = 0;  // packet counter, we increment per xmission

uint32_t localCounter = 0;

struct EvenDataPacket{
  uint32_t counter;
  float batteryVoltage;
  uint8_t cubeID;
  uint8_t side;
} eventData;

// Dont put this on the stack:
uint8_t eventBuffer[sizeof(eventData)];
uint8_t from;
uint8_t len = sizeof(eventData);


void selectRadio() {
  //digitalWrite(LED,HIGH);
  //digitalWrite(WIZ_CS, HIGH);
  //delay(100);
  digitalWrite(RFM69_CS, LOW);
  //delay(100);
}


//RF communication
void sendEventData()
{  
	selectRadio();
  rf69.send((uint8_t*)&eventData, sizeof(eventData));
	rf69.waitPacketSent();
}

void sendGoEvent()
{
  eventData.side = queueTail;
  eventData.cubeID = MY_ADDRESS;
  eventData.batteryVoltage = 0;
  eventData.counter++;
  Serial.print("About to send transmission number: ");
  Serial.println(eventData.counter);      
  sendEventData();
  //eventData.side = 0;
}




#define SHIELD_RESET  -1    // VS1053 reset pin (unused!)
#define SHIELD_CS     6     // VS1053 chip select pin (output)
#define SHIELD_DCS    10    // VS1053 Data/command select pin (output)
#define CARDCS        5     // Card chip select pin
#define DREQ          9     // VS1053 Data request, ideally an Interrupt pin

// DotStar Strip Size
#define NUMPIXELS 158 // 1 has 158, 2 has 139 // Number of LEDs in strip

// DotStar Control
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

  	//--Radio Setup--//
	selectRadio();
	//pinMode(LED, OUTPUT);     
	pinMode(RFM69_RST, OUTPUT);
	digitalWrite(RFM69_RST, LOW);

	Serial.println("Feather Addressed RFM69 TX Test!");
	Serial.println();

	// manual reset
	digitalWrite(RFM69_RST, HIGH);
	delay(50);
	digitalWrite(RFM69_RST, LOW);
	delay(50);
  
	if (!rf69_manager.init()) {
		Serial.println("RFM69 radio init failed");
		while (1);
	}
	Serial.println("RFM69 radio init OK!");
	// Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
	// No encryption
	if (!rf69.setFrequency(RF69_FREQ)) {
		Serial.println("setFrequency failed");
	}

	// If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
	// ishighpowermodule flag set like this:
	rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW

	// The encryption key has to be the same as the one in the server
	uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
					0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
	rf69.setEncryptionKey(key);  

	eventData.cubeID = 80; // 80 is the Marble Maze
	eventData.side = 0;
	eventData.batteryVoltage = 0;
	eventData.counter = 0;

  // CALIBRATION LOOP
  /*for(int cal=0; cal<4; cal++) {
    upAll();
    delay(3000);
    restAll();
    delay(3000);
  }*/

	Watchdog.enable(4000);

}


void loop()
{
    int audio_reading = analogRead(AUDIO_SENSE_PIN);
    //Serial.println(audio_reading);
    //delay(10);
    if (audio_reading > 22)
    {
        Serial.println(F("Playing Sound"));
        //colorWipe(GREEN, 5);
        musicPlayer.startPlayingFile("/track002.mp3");
        rainbow(1);
        colorWipe(strip.Color(0,0,0), 5);
        musicPlayer.stopPlaying();
    }
    Watchdog.reset();
}