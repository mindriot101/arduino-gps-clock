#include <NMEAGPS.h>
#include <GPSport.h>
#include <Streamers.h>
#include <HCMAX7219.h>
#include <SPI.h>


/* AltSoftSerial uses:
 *  
 *  TX 9
 *  RX 8
 */
 
#define LOAD 10

HCMAX7219 leds(LOAD);

static NMEAGPS gps;
static gps_fix fix;

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("GPS software start");

  Serial.print  ( F("\nGPS quiet time is assumed to begin after a ") );
  Serial.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
  Serial.println( F(" sentence is received.\n"
                    "  You should confirm this with NMEAorder.ino\n") );
  trace_header(Serial);
  Serial.flush();
  gpsPort.begin(9600);
}

void loop() {
  while (gps.available(gpsPort)) {
    fix = gps.read();
    NeoGPS::time_t dt = fix.dateTime;

    char buf[9];
    snprintf(buf, 9, "%02d.%02d.%02d",
      dt.hours,
      dt.minutes,
      dt.seconds);
    
    Serial.print("Time: ");
    Serial.println(buf);

    // Also update the display
    leds.Clear();
    leds.print7Seg(buf, 8);
    leds.Refresh();
  }
}
