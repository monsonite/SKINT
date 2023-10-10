# SKINT
Slightly Kludged MINT Interpreter in C++ for any Arduino compatible board


## MINT - A 5 Minute Guide.

MINT is a tiny, stack-based, bytecode interpreted language, loosely based on Forth.

It uses single, printable ascii characters to represent instructions, variables (registers) and User Routines.

There are 4 simple rules:

1. Instructions are almost always punctuation characters and symbols.

2. Variables are 16-bit wide and represented by lower case alpha characters a to z.

3. User Routines are assigned to uppercase characters A to Z and can be called just by typing the character, or executed in sequence by putting the characters into a string, such as ABC.

4. Numbers are input as decimal using the digits 0 to 9. A 16-bit integer between 0 and 65535 can be input as a string followed by a space used as a delimiter.


## Instruction Set.

The instruction primitives are represented by the 32 common punctuation and arithmetic symbols.

ADD   	+

SUB			-

MUL			*

DIV			/


GT			>

EQ			=

LT			<


BEGIN		(

END			)


AND			&

OR			|

XOR			^

INV			~

NEG			_


SHL 		{

SHR			}


HEXNUM	#


STRING	`Hello World`


DUP			"

DROP		'

SWAP		$

OVER		%


PRINT		.

HEXPRNT '

INPUT 	?



FETCH		@

STORE		!


DEF			:

ENDDEF	;


ARRAY   [	]


EXTEND 	\

