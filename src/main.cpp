 #include "Particle.h"
#include "temp_interface.h"
#include "temp_functions.h"

/***** DEFINES *****/
//#define DEBUG                       // debugging mode, flashes onboard LED
//#define SERIAL                     // sends serial information over USB
#define BAUD_RATE 9600              // baud rate for serial communication
#define INITIAL_PERIOD 30000        // default timer period / 2
                                    // 30000 = 30 seconds * 2 = 1 minute
#define MIN_PERIOD 1                // minimum timer period, in seconds
                                    // do not go below 1
#define MAX_SIZE 40                 // max size for char arrays

/***** GLOBAL VARIABLES *****/

// temperature. Declared here so it can be viewed in the cloud.
double temperature;

// state. controls whether we sample or convert
//      1 - convert state. The first timer trigger will switch this to zero
//      0 - sample state.
int state = 1;

// active. true if the interrupt triggered AND convert/sample haven't been
//  called since. false otherwise.
bool active = false;



/***** SAMPLE *****/
// Description: Reads temp value stored in sensor, then triggers the next sample
void sample() {
    // read temp
    temperature = (double) temp_read();

    #ifdef SERIAL
    if( temperature == 1 ) { // temp_read failed
        Serial.println("Temp read failed");
    }
    #endif

    if (temp_measure() == 1) { // temp_measure failed
        // if SERIAL is not defined, you may be worried that this "if" will
        //   be optimized away. C++11 standards prevent that from happening
        //   when temp_measure() is a non-empty function
        #ifdef SERIAL
        Serial.println("Temp measure failed");
        #endif
    }

}



/***** CONVERT *****/
// Description: Converts the temperature value set in SAMPLE, then
//   publishes the value.
void convert() {
    char publishString[MAX_SIZE]; // string for Particle.publish()

    temperature *= 0.0625; // multiply by precision to get C
    temperature = temperature * 1.8 + 32; // convert to F
    #ifdef SERIAL
    Serial.println(temperature);
    #endif

    // push to cloud
    sprintf(publishString, "%f", temperature);
    if( Particle.connected() ) { // verify we are connected first
        Particle.publish("pushToSheets", publishString, PRIVATE);
    }
}



/***** SAMPLE_OR_CONVERT *****/
// Description: Splits sampling and conversion into two separate functions. This
//   minimizes the long blocking delays created by float multiplication
void sample_or_convert() {
    if( state == 0) { // enter converting period
        state = 1;  // switch state value
        #ifdef DEBUG
        digitalWrite(D7,HIGH);
        #endif
    }
    else {          // enter sampling period
        state = 0; // switch state value
        #ifdef DEBUG
        digitalWrite(D7,LOW);
        #endif
    }
    active = true;
}



/***** TIMER SETTINGS *****/
// initial temperature sampling frequency. Can be updated via published function
// period = milliseconds / 2 (due to the separate sample/convert functions)
unsigned int period = INITIAL_PERIOD;

// Declare timer at initial period
Timer timer(period, sample_or_convert);

// function to change period of sampling timer. Input: String (in seconds)
// new_period = (input: seconds) * 500 (1000 milliseconds/2 due to
//                                      separate sample/convert functions)
int change_period( String new_period_str ) {
    unsigned int new_period = new_period_str.toInt();
    // error check our input String. string.toInt() returns 0 when not called on
    //   a convertible string, and values less than 1000ms do not allow
    //   sufficient conversion time for the DS18B20
    if( new_period < MIN_PERIOD ) {
        return 1;
    }
    else {
        // new period * 500 milliseconds
        // equivalent to (new_period in secons) * (1000 millis) / 2
        timer.changePeriod( new_period*500 );
    }
    return 0;
}



/***** SETUP *****/
void setup() {
    #ifdef SERIAL
    // start Serial, for communication
    Serial.begin(BAUD_RATE);
    #endif

    // Initialize temp sensor on pin D6
    temp_init(D6);

    #ifdef DEBUG
    // Set the onboard LED as output, initialize off
    pinMode(D7, OUTPUT);
    digitalWrite(D7, LOW);
    #endif

    // Add temperature variable to cloud
    // read on command line with `get /v1/devices/{DEVICE_ID}/temperature`
    Particle.variable("temperature", temperature);

    // Add change_period function to the cloud
    Particle.function("period_in_seconds", change_period);

    // start timer
    timer.start();

}



/***** LOOP *****/
void loop() {

    if( state == 0 && active ) {
        sample();   // record raw temperature, initiate next measurement
        active = false;
    }
    else if( state == 1 && active) {
        convert(); // convert temperature and publish
        active = false;
    }

}
