
/* Written by Oren Levy (auxren.com) based off some lackluster
 * documentation released by Intel.
 * MIDI over BLE info from: https://developer.apple.com/bluetooth/Apple-Bluetooth-Low-Energy-MIDI-Specification.pdf
 */
#include <CurieBLE.h>

#define TXRX_BUF_LEN              20 //max number of bytes
#define RX_BUF_LEN                20 //max number of bytes
uint8_t rx_buf[RX_BUF_LEN];
int rx_buf_num, rx_state = 0;
uint8_t rx_temp_buf[20];
uint8_t outBufMidi[128];

#define LOG_SERIAL Serial

//Buffer to hold 5 bytes of MIDI data. Note the timestamp is forced
uint8_t midiData[] = {0x80, 0x80, 0x00, 0x00, 0x00};

//Loads up buffer with values for note On
void noteOn(char chan, char note, char vel) //channel 1
{
  midiData[2] = 0x90 + chan;
  midiData[3] = note;
  midiData[4] = vel;
}

//Loads up buffer with values for note Off
void noteOff(char chan, char note) //channel 1
{
  midiData[2] = 0x80 + chan;
  midiData[3] = note;
  midiData[4] = 0;
}

BLEPeripheral midiDevice; // create peripheral instance

BLEService midiSvc("03B80E5A-EDE8-4B33-A751-6CE34EC4C700"); // create service

// create switch characteristic and allow remote device to read and write
BLECharacteristic midiChar("7772E5DB-3868-4112-A1A9-F2669D106BF3", BLEWrite | BLEWriteWithoutResponse | BLENotify | BLERead, 5);

void setup() {
  LOG_SERIAL.begin(9600);
  //  Set MIDI baud rate:
  Serial.begin(31250);
  BLESetup();
  LOG_SERIAL.println(("Bluetooth device active, waiting for connections..."));
}

void BLESetup()
{
  // set the local name peripheral advertises
  midiDevice.setLocalName("Auxren");
  midiDevice.setDeviceName("Auxren");

  // set the UUID for the service this peripheral advertises
  midiDevice.setAdvertisedServiceUuid(midiSvc.uuid());

  // add service and characteristic
  midiDevice.addAttribute(midiSvc);
  midiDevice.addAttribute(midiChar);

  // assign event handlers for connected, disconnected to peripheral
  midiDevice.setEventHandler(BLEConnected, midiDeviceConnectHandler);
  midiDevice.setEventHandler(BLEDisconnected, midiDeviceDisconnectHandler);

  // assign event handlers for characteristic
  midiChar.setEventHandler(BLEWritten, midiCharacteristicWritten);
  // set an initial value for the characteristic
  midiChar.setValue(midiData, 5);

  // advertise the service
  midiDevice.begin();
}

void loop() {

  /*Simple randome note player to test MIDI output
   * Plays random note every 400ms
   */
  int note = random(0, 127);
  //readMIDI();
  noteOn(0, note, 127); //loads up midiData buffer
  midiChar.setValue(midiData, 5);//midiData); //posts 5 bytes
  delay(200);
  noteOff(0, note);
  midiChar.setValue(midiData, 5);//midiData); //posts 5 bytes
  delay(200);
}


void midiDeviceConnectHandler(BLECentral& central) {
  // central connected event handler
  LOG_SERIAL.print("Connected event, central: ");
  LOG_SERIAL.println(central.address());
}

void midiDeviceDisconnectHandler(BLECentral& central) {
  // central disconnected event handler
  LOG_SERIAL.print("Disconnected event, central: ");
  LOG_SERIAL.println(central.address());
}

void midiCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic) {
  // central wrote new value to characteristic, update LED
  LOG_SERIAL.print("Characteristic event, written: ");
}
