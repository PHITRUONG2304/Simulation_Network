#ifndef BITOPERATIONS_H_
#define BITOPERATIONS_H_

/**
 * Set bit for 8-bit registers
 */
#define SET_BIT0(reg) reg |= 0b00000001
#define SET_BIT1(reg) reg |= 0b00000010
#define SET_BIT2(reg) reg |= 0b00000100
#define SET_BIT3(reg) reg |= 0b00001000
#define SET_BIT4(reg) reg |= 0b00010000
#define SET_BIT5(reg) reg |= 0b00100000
#define SET_BIT6(reg) reg |= 0b01000000
#define SET_BIT7(reg) reg |= 0b10000000

/**
 * Clear bit for 8-bit registers
 */

#define CLEAR_BIT0(reg) reg &= 0b11111110
#define CLEAR_BIT1(reg) reg &= 0b11111101
#define CLEAR_BIT2(reg) reg &= 0b11111011
#define CLEAR_BIT3(reg) reg &= 0b11110111
#define CLEAR_BIT4(reg) reg &= 0b11101111
#define CLEAR_BIT5(reg) reg &= 0b11011111
#define CLEAR_BIT6(reg) reg &= 0b10111111
#define CLEAR_BIT7(reg) reg &= 0b01111111

/**
 * Toggle bit for 8-bit registers
 */

#define TOGGLE_BIT0(reg) reg ^= 0b00000001
#define TOGGLE_BIT1(reg) reg ^= 0b00000010
#define TOGGLE_BIT2(reg) reg ^= 0b00000100
#define TOGGLE_BIT3(reg) reg ^= 0b00001000
#define TOGGLE_BIT4(reg) reg ^= 0b00010000
#define TOGGLE_BIT5(reg) reg ^= 0b00100000
#define TOGGLE_BIT6(reg) reg ^= 0b01000000
#define TOGGLE_BIT7(reg) reg ^= 0b10000000

/**
 * Get bit value for 8-bit registers
 */

#define BIT0_IS_SET(reg) (reg & 0b00000001)
#define BIT1_IS_SET(reg) (reg & 0b00000010)
#define BIT2_IS_SET(reg) (reg & 0b00000100)
#define BIT3_IS_SET(reg) (reg & 0b00001000)
#define BIT4_IS_SET(reg) (reg & 0b00010000)
#define BIT5_IS_SET(reg) (reg & 0b00100000)
#define BIT6_IS_SET(reg) (reg & 0b01000000)
#define BIT7_IS_SET(reg) (reg & 0b10000000)

/*
 * IO operation
 */

#define SET_IO_DIR_OUTPUT(X) _SET_IO_DIR_OUTPUT(X)
#define SET_IO_DIR_INTPUT(X) _SET_IO_DIR_INTPUT(X)
#define IO_WRITE_HIGH(X) _IO_WRITE_HIGH(X)
#define IO_WRITE_LOW(X) _IO_WRITE_LOW(X)
#define IO_TOGGLE(X) _IO_TOGGLE(X)

#define _SET_IO_DIR_OUTPUT(port, pin) SET_BIT##pin(DDR##port)
#define _SET_IO_DIR_INTPUT(port, pin) CLEAR_BIT##pin(DDR##port)
#define _IO_WRITE_HIGH(port, pin) SET_BIT##pin(PORT##port)
#define _IO_WRITE_LOW(port, pin) CLEAR_BIT##pin(PORT##port)
#define _IO_TOGGLE(port, pin) TOGGLE_BIT##pin(PORT##port)

#endif /* BITOPERATIONS_H_ */