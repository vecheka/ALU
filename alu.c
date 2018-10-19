/*
Program outline:
Simulates an ALU through use of code gates. The unit operates on 4 bit registers
and can perform addition with carry, logical AND, and logical NOT operations
on the registers.

Usage:
When running the program via console input, a switch is used to determine which
operation will be performed.

Hexadecimal values for input and output are formatted as "0x?" with the value
in place of ?

ADD: Use the "-a" flag followed by the two hexadecimal values to use as input.
AND: Use the "-d" flag followed by the two hexadecimal values to use as input.
NOT: Use the "-d" flag followed by one hexadecimal value to use as input.

Programmer: Vecheka Chhourn, Kameron Dodd

Date: 10/5/18

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef unsigned char Bit; // simulates a single bit, we'll use the low order bit in a byte

#define BIT0_MASK 0x01
#define REG_SIZE 4

Bit maskBit(Bit a) { a &= 0x01; return a; }

/* and
performs a bitwise AND on two operand bits
returns a single bit result
*/
Bit and(Bit a, Bit b) {
// truth table AND: 0 0 = 0, 0 1 = 0, 1 0 = 0, 1 1 = 1
	return (maskBit(a) == 1 && maskBit(b) == 1);
}

/* or
performs a bitwise OR on two operand bits
returns a single bit result
*/
Bit or(Bit a, Bit b) {
// truth table OR: 0 0 = 0, 0 1 = 1, 1 0 = 0, 1 1 = 1
	return (maskBit(a) == 1 || maskBit(b) == 1);
}

/* xor
performs a bitwise XOR on two operand bits
returns a single bit result
*/
Bit xor(Bit a, Bit b) {
// truth table XOR: 0 0 = 0, 0 1 = 1, 1 0 = 1, 1 1 = 0
	if (maskBit(a) == 1 && maskBit(b) == 0) return 1;
	else if (maskBit(b) == 1 && maskBit(a) == 0) return 1;
	else return 0;
}

/* not
performs a not operation on a single bit
returns a single bit result
*/
Bit not(Bit a) {
	return !(maskBit(a) == 1);
}

/**
Convert a character to a two-bit operation control code.
@param theOp - character representing the operation to perform
@param opReg - register to store the operation code
@return - the control code stored within the two least significant bits of a char
*/
unsigned char opDecode(char theOp) {
	unsigned char opcode = 0x03; // default to null operation
	if (theOp == 'a') { // ADD operation
		opcode &= 0x00;
	} else if (theOp == 'd') { // AND operation
		opcode &= 0x02;
	} else { // NOT operation
		opcode &= 0x01;
	}
	return opcode;
}

/**
Add two 4-bit binary
@param theA - register containing first binary number
@param theB - register containing second binary number
@param theResult - register to store resulting binary number
*/
void add(Bit theA[], Bit theB[], Bit theResult[]) {
    Bit carry = 0;
	int index = REG_SIZE - 1;
	while (index >= 0) {
		Bit sum = xor(theA[index], theB[index]);
		Bit r = xor(carry, sum);
		carry = or(and(carry, sum), and(theA[index], theB[index]));
		theResult[index--] = r;
    }
}

/** 
And two 4-bit binary
@param theA - register containing first binary number
@param theB - register containing second binary number
@param theResult - register to store resulting binary number
*/
void aluAnd(Bit theA[], Bit theB[], Bit theResult[]) {
    int i;
    for (i = 0; i < REG_SIZE; i++) {
		theResult[i] = and(theA[i], theB[i]);
    }
}

/**
Not a 4-bit binary
@param theBinary - register containing binary number
@param theResult - register to store resulting binary number
*/
void aluNot(Bit theBinary[], Bit theResult[]) {
    int i;
    for (i = 0; i < REG_SIZE; i++) {
      theResult[i] = not(theBinary[i]);
  }
}

/**
Find 4-bit binary of the given number
@param num - the given number
@param theBinary - register to store the 4-bit binary 
*/
void decimalToBinary(unsigned int num, Bit theBinary[]) {
    int index = REG_SIZE - 1;
    while (num > 0) {
		theBinary[index--] = num % 2;
		num /= 2;
    }
}

/**
Get the integer value of the give hex
@param theHex - hex character to interpret
@return - the decimal value of the character
*/
int getInteger(char theHex) {
	if (theHex < 'A') return theHex - '0';
	else return theHex - 'A' + 10;
}

/**
Convert binary stored in register to decimal value
@param theBinary - register to be converted
@return - the decimal value of the register's contents
*/
int binaryToDecimal(Bit theBinary[]) {
    int i;
    int num = 0;
    int deg = REG_SIZE - 1;
    for (i = 0; i < REG_SIZE; i++) {
		num += (1 << deg--) * theBinary[i];
    }
    return num;
}

/**
Find the hex representation of a decimal number
@param theNum - number to convert
@return - the hexadecimal value of the number as a character
*/
char getHexValue(int theNum) {
	char *hexValue = "0123456789ABCDEF";
	return hexValue[theNum];
}

/**
Find the two complement of the binary
@param theBinary - register to be complemented
@param theResult - register to store the result
*/
void twoComplement(Bit theBinary[], Bit theResult[]) {
	int i;
	Bit one[REG_SIZE] = {0};
	one[REG_SIZE - 1] = 1;
	for (i = 0; i < REG_SIZE; i++) {
		theBinary[i] = not(theBinary[i]);
	}
    add(theBinary, one, theResult);
}

// Driver for the operation of the simulated ALU
int main(int argc, char *argv[]) {
    Bit a[REG_SIZE] = {0};  	// ALU register a
    Bit b[REG_SIZE] = {0};		// ALU register b
    Bit r[REG_SIZE] = {0}; 		// ALU register r
    Bit twos [REG_SIZE] = {0};  // Temporary register for 2's complement
    unsigned char opcode = 0;
    int decimal;
    char hex;
    char flag = argv[1][1];
    int n1 = getInteger(argv[2][2]);
    int n2;
	
	// Convert operation flag to control code
	opcode = opDecode(flag);

    if (opcode == 0x00) { // Control code 0 0 denotes ADD
		n2 = getInteger(argv[3][2]);
		// Fill registers with binary representation
		decimalToBinary(n1, a);
		decimalToBinary(n2, b);
	
		add(a, b, r);

		hex = getHexValue(binaryToDecimal(r));
		if (r[0] == 1) {
			// Result is negative, use 2's complement when converting to decimal
			memcpy(twos, r, sizeof(r));
			twoComplement(twos, r);
			decimal = -binaryToDecimal(r);
		} else decimal = binaryToDecimal(r);

		printf("-> 0x%c = %d\n", hex, decimal);

    } else if (opcode == 0x02) { // Control code 1 0 denotes AND
		n2 = getInteger(argv[3][2]);
		// Fill registers with binary representation
		decimalToBinary(n1, a);
		decimalToBinary(n2, b);

		aluAnd(a, b, r);

		hex = getHexValue(binaryToDecimal(r));
		decimal = binaryToDecimal(r);
		printf("-> 0x%c = %d\n", hex, decimal);

    } else if (opcode == 0x01) { // Control code 0 1 denotes NOT
		// Fill register a with binary representation
		decimalToBinary(n1, a);
		
		aluNot(a, r);

		hex = getHexValue(binaryToDecimal(r));
		printf("-> 0x%c\n", hex);
    }
	
    return 0;
}