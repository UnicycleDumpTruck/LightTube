//  █████╗ ███╗   ██╗██╗███╗   ███╗ █████╗ ████████╗███████╗    ██╗     ██╗ ██████╗ ██╗  ██╗████████╗████████╗██╗   ██╗██████╗ ███████╗
// ██╔══██╗████╗  ██║██║████╗ ████║██╔══██╗╚══██╔══╝██╔════╝    ██║     ██║██╔════╝ ██║  ██║╚══██╔══╝╚══██╔══╝██║   ██║██╔══██╗██╔════╝
// ███████║██╔██╗ ██║██║██╔████╔██║███████║   ██║   █████╗      ██║     ██║██║  ███╗███████║   ██║      ██║   ██║   ██║██████╔╝█████╗  
// ██╔══██║██║╚██╗██║██║██║╚██╔╝██║██╔══██║   ██║   ██╔══╝      ██║     ██║██║   ██║██╔══██║   ██║      ██║   ██║   ██║██╔══██╗██╔══╝  
// ██║  ██║██║ ╚████║██║██║ ╚═╝ ██║██║  ██║   ██║   ███████╗    ███████╗██║╚██████╔╝██║  ██║   ██║      ██║   ╚██████╔╝██████╔╝███████╗
// ╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝╚═╝     ╚═╝╚═╝  ╚═╝   ╚═╝   ╚══════╝    ╚══════╝╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝      ╚═╝    ╚═════╝ ╚═════╝ ╚══════╝
                                                                                                                                    
#include <SD.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <Adafruit_DotStar.h>
#include <Adafruit_SleepyDog.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>


// ██████╗  █████╗ ██████╗ ██╗ ██████╗     ██████╗ ██╗███╗   ██╗███████╗
// ██╔══██╗██╔══██╗██╔══██╗██║██╔═══██╗    ██╔══██╗██║████╗  ██║██╔════╝
// ██████╔╝███████║██║  ██║██║██║   ██║    ██████╔╝██║██╔██╗ ██║███████╗
// ██╔══██╗██╔══██║██║  ██║██║██║   ██║    ██╔═══╝ ██║██║╚██╗██║╚════██║
// ██║  ██║██║  ██║██████╔╝██║╚██████╔╝    ██║     ██║██║ ╚████║███████║
// ╚═╝  ╚═╝╚═╝  ╚═╝╚═════╝ ╚═╝ ╚═════╝     ╚═╝     ╚═╝╚═╝  ╚═══╝╚══════╝
                                                                     
#define MY_ADDRESS    86 // Station 1 is 85, Station 2 is 86
#define RFM69_CS      A4
#define RFM69_INT     A3
#define RFM69_RST     11
#define RF69_FREQ     915.0


//  █████╗ ██╗   ██╗██████╗ ██╗ ██████╗     ██████╗ ██╗███╗   ██╗███████╗
// ██╔══██╗██║   ██║██╔══██╗██║██╔═══██╗    ██╔══██╗██║████╗  ██║██╔════╝
// ███████║██║   ██║██║  ██║██║██║   ██║    ██████╔╝██║██╔██╗ ██║███████╗
// ██╔══██║██║   ██║██║  ██║██║██║   ██║    ██╔═══╝ ██║██║╚██╗██║╚════██║
// ██║  ██║╚██████╔╝██████╔╝██║╚██████╔╝    ██║     ██║██║ ╚████║███████║
// ╚═╝  ╚═╝ ╚═════╝ ╚═════╝ ╚═╝ ╚═════╝     ╚═╝     ╚═╝╚═╝  ╚═══╝╚══════╝
                                                                      
#define AUDIO_SENSE_PIN A0
#define SHIELD_RESET    -1    // VS1053 reset pin (unused!)
#define SHIELD_CS       6     // VS1053 chip select pin (output)
#define SHIELD_DCS      10    // VS1053 Data/command select pin (output)
#define CARDCS          5     // Card chip select pin
#define DREQ            9     // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);


// ██╗     ███████╗██████╗     ██████╗ ██╗███╗   ██╗███████╗
// ██║     ██╔════╝██╔══██╗    ██╔══██╗██║████╗  ██║██╔════╝
// ██║     █████╗  ██║  ██║    ██████╔╝██║██╔██╗ ██║███████╗
// ██║     ██╔══╝  ██║  ██║    ██╔═══╝ ██║██║╚██╗██║╚════██║
// ███████╗███████╗██████╔╝    ██║     ██║██║ ╚████║███████║
// ╚══════╝╚══════╝╚═════╝     ╚═╝     ╚═╝╚═╝  ╚═══╝╚══════╝
                                                         
#define DATAPIN A1 // Station 2 is A1, Station 1 is A2
#define CLOCKPIN A2 // Station 2 is A2, Station 1 is A1
#define NUMPIXELS 139 // 1 has 158, 2 has 139 // Number of LEDs in strip


// ██████╗  █████╗ ██████╗ ██╗ ██████╗     ███████╗██╗   ██╗███╗   ██╗ ██████╗████████╗██╗ ██████╗ ███╗   ██╗███████╗
// ██╔══██╗██╔══██╗██╔══██╗██║██╔═══██╗    ██╔════╝██║   ██║████╗  ██║██╔════╝╚══██╔══╝██║██╔═══██╗████╗  ██║██╔════╝
// ██████╔╝███████║██║  ██║██║██║   ██║    █████╗  ██║   ██║██╔██╗ ██║██║        ██║   ██║██║   ██║██╔██╗ ██║███████╗
// ██╔══██╗██╔══██║██║  ██║██║██║   ██║    ██╔══╝  ██║   ██║██║╚██╗██║██║        ██║   ██║██║   ██║██║╚██╗██║╚════██║
// ██║  ██║██║  ██║██████╔╝██║╚██████╔╝    ██║     ╚██████╔╝██║ ╚████║╚██████╗   ██║   ██║╚██████╔╝██║ ╚████║███████║
// ╚═╝  ╚═╝╚═╝  ╚═╝╚═════╝ ╚═╝ ╚═════╝     ╚═╝      ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝
                                                                                                                  
// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);

struct EvenDataPacket{
  uint32_t counter;
  float batteryVoltage;
  uint8_t cubeID;
  uint8_t side;
} eventData;

uint8_t eventBuffer[sizeof(eventData)];
uint8_t from;
uint8_t len = sizeof(eventData);
int16_t packetnum = 0;  // packet counter, we increment per xmission
uint32_t localCounter = 0;

void sendGoEvent(uint8_t s)
{
  eventData.side = s;
  eventData.cubeID = MY_ADDRESS;
  eventData.batteryVoltage = 0;
  eventData.counter++;
  Serial.print("About to send transmission number: ");
  Serial.println(eventData.counter);      
  digitalWrite(RFM69_CS, LOW);
  delay(50);
  rf69.send((uint8_t*)&eventData, sizeof(eventData));
	rf69.waitPacketSent();
  delay(50);
  digitalWrite(RFM69_CS, HIGH);
}

void radioSetup() {
  pinMode(RFM69_CS, OUTPUT); // This line caused the audio shield to not be found?
  // pinMode(RFM69_EN, OUTPUT);
  pinMode(RFM69_RST, OUTPUT);

  digitalWrite(RFM69_CS, LOW);
  //digitalWrite(RFM69_EN, HIGH);
	digitalWrite(RFM69_RST, LOW);
  delay(50);
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

	eventData.counter = 0;

  sendGoEvent(0);
  digitalWrite(RFM69_CS, HIGH);
  Serial.println("Radio Setup Complete");
}


//  █████╗ ██╗   ██╗██████╗ ██╗ ██████╗     ███████╗███████╗████████╗██╗   ██╗██████╗ 
// ██╔══██╗██║   ██║██╔══██╗██║██╔═══██╗    ██╔════╝██╔════╝╚══██╔══╝██║   ██║██╔══██╗
// ███████║██║   ██║██║  ██║██║██║   ██║    ███████╗█████╗     ██║   ██║   ██║██████╔╝
// ██╔══██║██║   ██║██║  ██║██║██║   ██║    ╚════██║██╔══╝     ██║   ██║   ██║██╔═══╝ 
// ██║  ██║╚██████╔╝██████╔╝██║╚██████╔╝    ███████║███████╗   ██║   ╚██████╔╝██║     
// ╚═╝  ╚═╝ ╚═════╝ ╚═════╝ ╚═╝ ╚═════╝     ╚══════╝╚══════╝   ╚═╝    ╚═════╝ ╚═╝     
                                                                                   
void vsAudioSetup() {
  pinMode(CARDCS, OUTPUT);
  pinMode(SHIELD_CS, OUTPUT);
  
  digitalWrite(SHIELD_CS, LOW);
  delay(50);

  if (! musicPlayer.begin()) { // initialise the music player
      Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
      while (1);
  }
  digitalWrite(SHIELD_CS, HIGH);
  Serial.println(F("VS1053 found"));
  
  digitalWrite(CARDCS, LOW);
  if (!SD.begin(CARDCS)) {
      Serial.println(F("SD failed, or not present"));
      while (1);  // don't do anything more
  }
  digitalWrite(CARDCS, HIGH);
  
  // Set volume for left, right channels. lower numbers == louder volume!
  digitalWrite(SHIELD_CS, LOW);
  musicPlayer.setVolume(0,0);
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
  digitalWrite(SHIELD_CS, HIGH);
}

// ██╗     ███████╗██████╗     ███████╗██╗   ██╗███╗   ██╗ ██████╗████████╗██╗ ██████╗ ███╗   ██╗███████╗
// ██║     ██╔════╝██╔══██╗    ██╔════╝██║   ██║████╗  ██║██╔════╝╚══██╔══╝██║██╔═══██╗████╗  ██║██╔════╝
// ██║     █████╗  ██║  ██║    █████╗  ██║   ██║██╔██╗ ██║██║        ██║   ██║██║   ██║██╔██╗ ██║███████╗
// ██║     ██╔══╝  ██║  ██║    ██╔══╝  ██║   ██║██║╚██╗██║██║        ██║   ██║██║   ██║██║╚██╗██║╚════██║
// ███████╗███████╗██████╔╝    ██║     ╚██████╔╝██║ ╚████║╚██████╗   ██║   ██║╚██████╔╝██║ ╚████║███████║
// ╚══════╝╚══════╝╚═════╝     ╚═╝      ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝
                                                                                                      
// DotStar Control
Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);

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
    // Watchdog.reset();
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


// ███████╗███████╗████████╗██╗   ██╗██████╗ 
// ██╔════╝██╔════╝╚══██╔══╝██║   ██║██╔══██╗
// ███████╗█████╗     ██║   ██║   ██║██████╔╝
// ╚════██║██╔══╝     ██║   ██║   ██║██╔═══╝ 
// ███████║███████╗   ██║   ╚██████╔╝██║     
// ╚══════╝╚══════╝   ╚═╝    ╚═════╝ ╚═╝     
                                          
void setup()
{
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
  
  pinMode(AUDIO_SENSE_PIN, INPUT);

  Serial.begin(9600);
  // while (!Serial) {
  //     ; // wait for serial port to connect. Needed for native USB port only
  // }
  Serial.println("LightTube setup function commencing...");
  
  // radioSetup();
  
  // delay(100);

  vsAudioSetup();

  delay(100);

  radioSetup();

	// Watchdog.enable(4000);
  Serial.println("Setup Complete");
}


// ██╗      ██████╗  ██████╗ ██████╗ 
// ██║     ██╔═══██╗██╔═══██╗██╔══██╗
// ██║     ██║   ██║██║   ██║██████╔╝
// ██║     ██║   ██║██║   ██║██╔═══╝ 
// ███████╗╚██████╔╝╚██████╔╝██║     
// ╚══════╝ ╚═════╝  ╚═════╝ ╚═╝     
                                  
void loop()
{
    int audio_reading = analogRead(AUDIO_SENSE_PIN);
    //Serial.println(audio_reading);
    // delay(10);
    if (audio_reading > 22)
    {
        Serial.println(F("Playing Sound"));
        digitalWrite(SHIELD_CS, LOW);
        musicPlayer.startPlayingFile("/track002.mp3");
        digitalWrite(SHIELD_CS, HIGH);
        rainbow(1);
        sendGoEvent(1);
        colorWipe(strip.Color(0,0,0), 5);
        digitalWrite(SHIELD_CS, LOW);
        musicPlayer.stopPlaying();
        digitalWrite(SHIELD_CS, HIGH);
    }
    // Watchdog.reset();
}