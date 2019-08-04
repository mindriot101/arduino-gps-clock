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

// Mode select options
enum class Mode {
    Time,
    Satellites,
    Location,
};

static NMEAGPS gps;
static gps_fix fix;
static Mode MODE;

void setup() {
    Serial.begin(9600);
    while (!Serial)
        ;
    Serial.println("GPS software start");

    Serial.print  ( F("\nGPS quiet time is assumed to begin after a ") );
    Serial.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
    Serial.println( F(" sentence is received.\n"
                "  You should confirm this with NMEAorder.ino\n") );
    Serial.flush();

    gpsPort.begin(9600);

    // Select the mode we're in
    MODE = Mode::Time;
}

void loop() {
    while (gps.available(gpsPort)) {
        fix = gps.read();

        switch(MODE) {
            case Mode::Time:
                NeoGPS::time_t dt = fix.dateTime;

                char buf[9];
                snprintf(buf, 9, "%02d.%02d.%02d",
                        dt.hours,
                        dt.minutes,
                        dt.seconds);

#ifndef NDEBUG
                Serial.print("Time: ");
                Serial.println(buf);
#endif

                // Also update the display
                leds.Clear();
                leds.print7Seg(buf, 8);
                leds.Refresh();
                break;
            case Mode::Satellites:
                Serial.println("Satellites mode not handled yet");
                break;
            case Mode::Location:
                Serial.println("Location mode not handled yet");
                break;
        }

    }
}
