/*
Pozyx tests for max range
Juan L. Pérez Díez

*/
#include <Pozyx.h>
#include <Pozyx_definitions.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_SPIFlash.h>
#include <Adafruit_SPIFlash_FatFs.h>
#include <Adafruit_NeoPixel.h>

//SPI Flash 2MB memory
#define FLASH_TYPE	SPIFLASHTYPE_W25Q16BV
#define FLASH_SS       SS1                    // Flash chip SS pin.
#define FLASH_SPI_PORT SPI1                   // What SPI port is Flash on?
Adafruit_SPIFlash flash(FLASH_SS, &FLASH_SPI_PORT);     // Use hardware SPI
Adafruit_W25Q16BV_FatFs fatfs(flash);

//NeoPixel
const uint8_t neoPixelPin = 40;
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, neoPixelPin, NEO_GRB + NEO_KHZ800);

//Pozyx ID
uint16_t myID = 0; 

void setup() {
	boolean failInit = false;

	Serial.begin(115200);
	//Waits for usb connection before continuing
	//while (!Serial);
	Serial.println();
	Serial.println(".::[ Pozyx max range experiment - Juan L. Pérez Díez ]::.");

	// Initialize flash library and check its chip ID.
	if (!flash.begin(FLASH_TYPE)) {
		Serial.println(F("ERROR:, failed to initialize flash chip!"));
		failInit = true;
	} else {
		Serial.print(F("Flash chip JEDEC ID: 0x")); 
		Serial.println(flash.GetJEDECID(), HEX);
	}

	//Init filesystem
	if (!fatfs.begin()) {
		Serial.println(F("ERROR: failed to mount newly formatted filesystem!"));
		Serial.println(F("Was the flash chip formatted with the fatfs_format example?"));
		failInit = true;
	} else
	Serial.println(F("Mounted filesystem!"));

	//Init pozyx
	if(Pozyx.begin() == POZYX_FAILURE) {
		Serial.println(F("ERROR: Unable to connect to pozyx shield"));
    	failInit = true;
	} else {
		// read the network id of this device
  		Pozyx.regRead(POZYX_NETWORK_ID, (uint8_t*)&myID, 2);
  		Serial.print("Pozyx ID: ");
  		Serial.println(myID, HEX);
  	}

  	pixel.begin();
	if (failInit) {
		//Purple led
		pixel.setPixelColor(0, pixel.Color(255,0,255));
    	pixel.show();
		while(1);
	} else {
		//Green led
		pixel.setPixelColor(0, pixel.Color(0,255,0));
    	pixel.show();
    	Serial.println("Setup went OK");
    }
}

void loop() {
	rainbow(10);
	Serial.print("-");
}


//Rainbox effect for NeoPixel
void rainbow(uint8_t wait) {
  uint16_t j;

  for(j=0; j<256; j++) {
    pixel.setPixelColor(0, Wheel((j) & 255));
    pixel.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixel.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixel.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixel.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

/*

#include <Pozyx.h>
#include <Pozyx_definitions.h>
#include <Wire.h>

uint16_t source_id;                 // the network id of this device
uint16_t destination_id = 0;        // the destination network id. 0 means the message is broadcasted to every device in range
String inputString = "";            // a string to hold incoming data
boolean stringComplete = false;     // whether the string is complete

void setup(){
  Serial.begin(115200);

  // initialize Pozyx
  if(! Pozyx.begin(false, MODE_INTERRUPT, POZYX_INT_MASK_RX_DATA, 0)){
    Serial.println("ERROR: Unable to connect to POZYX shield");
    Serial.println("Reset required");
    abort();
  }

  // read the network id of this device
  Pozyx.regRead(POZYX_NETWORK_ID, (uint8_t*)&source_id, 2);

  // reserve 100 bytes for the inputString:
  inputString.reserve(100);

  Serial.println("--- Pozyx Chat started ---");
}

void loop(){

  // check if we received a newline character and if so, broadcast the inputString.
  if(stringComplete){
    Serial.print("Ox");
    Serial.print(source_id, HEX);
    Serial.print(": ");
    Serial.println(inputString);

    int length = inputString.length();
    uint8_t buffer[length];
    inputString.getBytes(buffer, length);

    // write the message to the transmit (TX) buffer
    int status = Pozyx.writeTXBufferData(buffer, length);
    // broadcast the contents of the TX buffer
    status = Pozyx.sendTXBufferData(destination_id);

    inputString = "";
    stringComplete = false;
  }

  // we wait up to 50ms to see if we have received an incoming message (if so we receive an RX_DATA interrupt)
  if(Pozyx.waitForFlag(POZYX_INT_STATUS_RX_DATA,50))
  {
    // we have received a message!

    uint8_t length = 0;
    uint16_t messenger = 0x00;
    delay(1);
    // Let's read out some information about the message (i.e., how many bytes did we receive and who sent the message)
    Pozyx.getLastDataLength(&length);
    Pozyx.getLastNetworkId(&messenger);

    char data[length];

    // read the contents of the receive (RX) buffer, this is the message that was sent to this device
    Pozyx.readRXBufferData((uint8_t *) data, length);
    Serial.print("Ox");
    Serial.print(messenger, HEX);
    Serial.print(": ");
    Serial.println(data);
  }

}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();

    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it.
    // otherwise, add it to the inputString:
    if (inChar == '\n') {
      stringComplete = true;
    }else{
      inputString += inChar;
    }
  }
}

*/