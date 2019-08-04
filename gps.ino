#include <GPSport.h>
#include <HCMAX7219.h>
#include <NMEAGPS.h>
#include <SPI.h>
#include <Streamers.h>

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
    Latitude,
    Longitude,
    Altitude,
};
#define MODE_PIN 2

static NMEAGPS gps;
volatile Mode current_mode = Mode::Time;
static char print_buffer[9];

void updateMode() {
    Serial.println("Interrupt fired");

    switch (current_mode) {
        case Mode::Time:
            Serial.println("Time -> Satellites");
            current_mode = Mode::Satellites;
            break;
        case Mode::Satellites:
            Serial.println("Satellites -> Latitude");
            current_mode = Mode::Latitude;
            break;
        case Mode::Latitude:
            Serial.println("Latitude -> Longitude");
            current_mode = Mode::Longitude;
            break;
        case Mode::Longitude:
            Serial.println("Longitude -> Altitude");
            current_mode = Mode::Altitude;
            break;
        case Mode::Altitude:
            Serial.println("Altitude -> Time");
            current_mode = Mode::Time;
            break;
    }
}

void setup() {
    Serial.begin(9600);
    while (!Serial)
        ;
    Serial.println("GPS software start");

    Serial.print(F("\nGPS quiet time is assumed to begin after a "));
    Serial.print(gps.string_for(LAST_SENTENCE_IN_INTERVAL));
    Serial.println(F(" sentence is received.\n"
                "  You should confirm this with NMEAorder.ino\n"));
    Serial.flush();

    gpsPort.begin(9600);

    // Select the mode we're in
    attachInterrupt(digitalPinToInterrupt(MODE_PIN), updateMode, RISING);
}

void loop() {
    while (gps.available(gpsPort)) {
        const gps_fix fix = gps.read();

        if (current_mode == Mode::Time) {
            Serial.println("In time mode");
            NeoGPS::time_t dt = fix.dateTime;

            snprintf(print_buffer, 9, "%02d.%02d.%02d", dt.hours, dt.minutes,
                    dt.seconds);

        } else if (current_mode == Mode::Satellites) {
            Serial.println("In satellite mode");
            snprintf(print_buffer, 9, "sats %3d", fix.satellites);
            Serial.println(print_buffer);

        } else if (current_mode == Mode::Latitude) {
            Serial.println("In latitude mode");

            float lat = fix.latitude();
            int32_t lat_deg = int32_t(lat);
            int32_t rem = int32_t((lat - lat_deg) * 100);

            snprintf(print_buffer, 9, "lat %2d.%2d", lat_deg, rem);
        } else if (current_mode == Mode::Longitude) {
            Serial.println("In longitude mode");

            float lng = fix.longitude();
            int32_t lng_deg = int32_t(lng);
            int32_t rem = int32_t((lng - lng_deg) * 100);

            snprintf(print_buffer, 9, "lng %2d.%2d", lng_deg, rem);
        } else if (current_mode == Mode::Altitude) {
            Serial.println("In altitude mode");

            float alt = fix.altitude();

            char str_temp[6];
            dtostrf(alt, 3, 2, str_temp);
            snprintf(print_buffer, 9, "alt %s", str_temp);
        } else {
            Serial.println("Not in a known mode");
        }

        leds.Clear();
        leds.print7Seg(print_buffer, 8);
        leds.Refresh();

        Serial.println(print_buffer);
    }
}
