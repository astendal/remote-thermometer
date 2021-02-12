#ifndef TEMP_FUNCTIONS_H
#define	TEMP_FUNCTIONS_H

// DESCRIPTION:
// The two standard operations, TEMP_MEASURE and TEMP_READ are implemented
//   by this library. Other commands defined in the DS18B20 manual can be
//   implemented by calls to TEMP_READBYTES and TEMP_WRITEBYTES.
// Custom commands must be implemented using the following structure:
//   temp_resetDevice()
//   temp_writeBits(0xcc)           (0xnn, Skip Rom, addresses a single device)
//   temp_writeBits(0xnn)           (0xnn, replace with alternate command)
// DEPENDENCIES: Particle.h temp_interface.h


/***** TEMP_MEASURE *****/
int temp_measure();
// DESCRIPTION: Sends the Convert T command to the DS18B20, initiating a
//              single temperature measurement.
// RETURN: 1 if device sent a presence pulse, 0 if error


/***** TEMP_READ *****/
int temp_read();
// DESCRIPTION: Reads converted temperature from DS18B20
// RETURN: int representing temperature recorded by DS18B20
//         returns 1 if error occured 
//         divide by the float corresponding to the precision selected below
//         to get a return value in degrees celcius
//         +------------------------------
//         PRECISION        ---- ACCURACY (C) --------- CONVERSION TIME (ms)
//         12-bit (default) ----  0.0625      ---------       750
//         11-bit           ----  0.125       ---------       375
//         10-bit           ----  0.25        ---------       187.5
//          9-bit           ----  0.5         ---------       93.75


#endif	/* TEMP_FUNCTIONS_H */
