#include "Particle.h"
#include "temp_interface.h"


/***** TEMP_MEASURE *****/
// DESCRIPTION: Sends the Convert T command to the DS18B20, initiating a
//              single temperature measurement.
// RETURN: 1 if device sent a presence pulse, 0 if error
int temp_measure() {       // tells temperature sensor to measure and start converting
    int err = temp_reset();     // necessary to dance with the temperature sensor before commanding
    temp_writeBits(8, 0xcc);  // skip ROM, address devices generally
    temp_writeBits(8, 0x44);  // convert t, records and converts temperature
    return err;
}


/***** TEMP_READ *****/
// DESCRIPTION: Reads converted temperature from DS18B20
// RETURN: int representing temperature recorded by DS18B20
//         returns 1 if error occured.
//         divide by the float corresponding to the precision selected below
//         to get a return value in degrees celcius
//         +------------------------------
//         PRECISION        ---- ACCURACY (C) --------- CONVERSION TIME (ms)
//         12-bit (default) ----  0.0625      ---------       750
//         11-bit           ----  0.125       ---------       375
//         10-bit           ----  0.25        ---------       187.5
//          9-bit           ----  0.5         ---------       93.75
int temp_read() {
    int val = 0;            // stores value from temp_readBits until it can be added to the buffer
    val = temp_reset();     // necessary to dance with the temperature sensor before commanding
    if( val == 1 ) { // error has occured
        return val;
    }
    temp_writeBits(8, 0xcc);  // skip ROM, address devices generally
    temp_writeBits(8, 0xbe);  // read scratchpad from temperature sensor
    val = temp_readBits(16); // read 16 bit temperature value

    return val;
}
