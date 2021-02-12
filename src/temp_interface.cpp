#include "Particle.h"


////////////////////////
// INTERNAL FUNCTIONS //
////////////////////////

// pin that we communicate on. Used by pinMode, digitalWrite, and digitalRead
// set in TEMP_INIT
uint16_t pin;


/***** TEMP_SETTOINPUT *****/
// DESCRIPTION: sets DS18B20 pin to input with pull-up resistor
void temp_setToInput() {
    pinMode(pin, INPUT_PULLUP);
}


/***** TEMP_SETTOOUTPUT *****/
// DESCRIPTION: sets DS18B20 pin to output
void temp_setToOutput() {
    pinMode(pin, OUTPUT);
}


/***** TEMP_DRIVELOW *****/
// DESCRIPTION: assumes DS18B20 pin is set as output with pull-up
//              writes LOW signal to pin
void temp_driveLow() {
    digitalWrite(pin, LOW);
}


/***** TEMP_DRIVEHIGH *****/
// DESCRIPTION: assumes DS18B20 pin is set to output with pull-up
//              writes HIGH signal to pin
void temp_driveHigh() {
    digitalWrite(pin, HIGH);
}


/***** TEMP_READPIN *****/
// DESCRIPTION: assumes DS18B20 pin is set to input
//              returns binary integer value
int temp_readPin() {
    return digitalRead(pin);
}


/***** TEMP_WRITE0 *****/
// DESCRIPTION: assumes DS18B20 pin is set to output with pull-up
//              sends pulses indicating a 0 bit to the DS18B20
void temp_write0() {
    temp_driveLow();
    delayMicroseconds(60);
    temp_driveHigh();
    delayMicroseconds(10);
}


/***** TEMP_WRITE1 *****/
// DESCRIPTION: assumes DS18B20 pin is set to output with pull-up
//              sends pulses indicating a 1 bit to the DS18B20
void temp_write1() {
    temp_driveLow();
    delayMicroseconds(6);
    temp_driveHigh();
    delayMicroseconds(64);
}


/***** TEMP_WRITEBIT *****/
// DESCRIPTION: assumes DS18B20 pin is set to output with pull-up
//              wraps TEMP_WRITE0 and TEMP_WRITE1 into a single function call
void temp_writeBit( int n ) {
    if( n == 1 )
        temp_write1();
    else
        temp_write0();
}


/***** TEMP_READBIT *****/
// DESCRIPTION: assumes DS18B20 pin is set to output with pull-up
//              reads and returns a bit from the DS18B20
int temp_readBit() {
    int val;

    // create read time-slot
    temp_driveLow();
    delayMicroseconds(6);
    temp_driveHigh();
    delayMicroseconds(6);

    // now that read time slot is created, we will sample our pin
    temp_setToInput();
    val = temp_readPin();
    delayMicroseconds(55);
    temp_setToOutput();     // return to default state

    return val;
}


////////////////////////
// EXTERNAL FUNCTIONS //
////////////////////////


/***** TEMP_RESET *****/
// DESCRIPTION: Resets DS18B20, primes for communication.
// RETURN: 1 if device sent a presence pulse, 0 if error
int temp_reset() {
    // reset the temperature sensor

    // store presence check value
    int err;

    temp_setToOutput();
    temp_driveLow();
    delayMicroseconds(480);     // hold in reset
    temp_driveHigh();
    delayMicroseconds(70);          // return high, wait.

    temp_setToInput();
    if( temp_readPin() == 0 ) {   // check for acknowledge driveLow from temp sensor
        err = 0;
    }
    else { // acknoledge not received
        err = 1;
    }

    delayMicroseconds(410);         // wait for temp sensor to finish initializing
    temp_setToOutput();         // default state for master

    return err;
}


/***** TEMP_INIT *****/
// DESCRIPTION: Sets pin mode, checks if DS18B20 is responding
// RETURN: 1 if device sent a presence pulse, 0 if error
int temp_init( uint16_t pin_input ) {
    // set pin variable to input
    pin = pin_input;
    // our logic requires the pin be tied to the pull-up resistor
    pinMode(pin, INPUT_PULLUP);
    return (temp_reset());
}


/***** TEMP_WRITEBITS *****/
// DESCRIPTION: Writes an command of the specified size to the DS18B20. Follow
//              custom command instructions at the top of this page
void temp_writeBits( int size, int command ) { // TODO: check this logic
    for( int k = 0; k < size; k++ ) {
        temp_writeBit( (command >> k) & 1 ); // writes size times, one bit at a time.
    }
}


/***** TEMP_READBITS *****/
// DESCRIPTION: Reads a value of the specified size from the DS18B20. Follow
//              custom command instructions at the top of this page.
int temp_readBits( int size ) {
    // build integer one bit at a time by doing readBit size times. Then return this value.
    int val = 0;

    for( int k = 0; k < size; k++ ) { // TODO: check why this is 16 and not 8
        val = val | (temp_readBit() << k);      // add up to 8 ones to val
    }

    return val;
}
