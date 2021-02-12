#ifndef TEMP_INTERFACE_H
#define	TEMP_INTERFACE_H

// DESCRIPTION:
// This library handles interfacing with the DS18B20 temperature sensor
//   via calls to delayMicroseconds(). The maximum blocking of any one of
//   the following functions does not exceed 1ms.
// DEPENDENCIES: Particle.h


/***** temp_reset *****/
int temp_reset();
// DESCRIPTION: Resets DS18B20, primes for communication.
// RETURN: 1 if device sent a presence pulse, 0 if error


/***** TEMP_INIT *****/
int temp_init( uint16_t pin_input );
// DESCRIPTION: Sets pin and pin mode, checks if DS18B20 is responding
// RETURN: 1 if device sent a presence pulse, 0 if error


/***** TEMP_WRITEBITS *****/
void temp_writeBits( int size, int command );
// DESCRIPTION: Writes an command of the specified size to the DS18B20. Follow
//              custom command instructions at the top of this page


/***** TEMP_READBITS *****/
int temp_readBits( int size );
// DESCRIPTION: Reads a value of the specified size from the DS18B20. Follow
//              custom command instructions at the top of this page.


#endif	/* TEMP_INTERFACE_H */
