/*
 * BinClock.c
 * Jarrod Olivier
 * Modified for EEE3095S/3096S by Keegan Crankshaw
 * August 2019
 * 
 * <STUDNUM_1> <STUDNUM_2>
 * Date
*/

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h> //For printf functions
#include <stdlib.h> // For system functions

#include "BinClock.h"
#include "CurrentTime.h"

//Global variables
int hours, mins, secs;
long lastInterruptTime = 0; //Used for button debounce
int RTC; //Holds the RTC instance

int HH,MM,SS;
int softPwmCreate(int pin, int initialValue, int pwmRange );
void softPwmWrite(int pin, int value);


void initGPIO(void){
	/* 
	 * Sets GPIO using wiringPi pins. see pinout.xyz for specific wiringPi pins
	 * You can also use "gpio readall" in the command line to get the pins
	 * Note: wiringPi does not use GPIO or board pin numbers (unless specifically set to that mode)
	 */
	printf("Setting up\n");
	wiringPiSetup(); //This is the default mode. If you want to change pinouts, be aware
	
	RTC = wiringPiI2CSetup(RTCAddr); //Set up the RTC
	
	//Set up the LEDS
	for(int i=0; i < sizeof(LEDS)/sizeof(LEDS[0]); i++){
	    pinMode(LEDS[i], OUTPUT);
		//digitalWrite(LEDS[i] , HIGH);
		
		delay(500);
		//digitalWrite(LEDS[i] , LOW);
		
	}
	
	//Set Up the Seconds LED for PWM
	//Write your logic here
	pinMode(SECS,PWM_OUTPUT);
	softPwmCreate(SECS, 0, 100);

	


	
	printf("LEDS done\n");

	//lightHours(12);
	
	//Set up the Buttons
	for(int j=0; j < sizeof(BTNS)/sizeof(BTNS[0]); j++){
		pinMode(BTNS[j], INPUT);
		pullUpDnControl(BTNS[j], PUD_UP);


	}
	
	//Attach interrupts to Buttons
	//Write your logic here
	void interrupt1(void){
		wiringPiISR(BTNS[0], INT_EDGE_RISING,&interrupt1);
		wiringPiISR(BTNS[1], INT_EDGE_RISING,&interrupt1);
	
	}
	
	printf("BTNS done\n");
	printf("Setup done\n");
}


/*
 * The main function
 * This function is called, and calls all relevant functions we've written
 */
int main(void){
	initGPIO();

	//Set random time (3:04PM)
	//You can comment this file out later
	//  wiringPiI2CWriteReg8(RTC, HOUR, 0x13+TIMEZONE);
	//  wiringPiI2CWriteReg8(RTC, MIN, 0x4);
	// wiringPiI2CWriteReg8(RTC, SEC, 0x80);
	toggleTime();
	
	// Repeat this until we shut down
	for (;;){
		//Fetch the time from the RTC
		//Write your logic here
		
			hours = wiringPiI2CReadReg8(RTC, HOUR);
			mins = wiringPiI2CReadReg8(RTC, MIN);
			secs= wiringPiI2CReadReg8(RTC, SEC)-0x80;
		// hours= getHours();
		// mins = getMins();
		// secs = getSecs();
		//Function calls to toggle LEDs
		//Write your logic here
			
		//printf("btn 0 has: "+digitalRead( BTNS[0]));
		// Print out the time we have stored on our RTC
		
		
		lightHours(hexCompensation(hours));
		lightMins(hexCompensation(mins));
		secPWM(hexCompensation(secs));
	
		printf("The current time is: %x:%x:%x\n", hours, mins, secs);

		
		
		//using a delay to make our program "less CPU hungry"
		delay(1000); //milliseconds
	}
	return 0;
}

/*
 * Change the hour format to 12 hours
 */
int hFormat(int hours){
	/*formats to 12h*/
	if (hours >= 24){
		hours = 0;
	}
	else if (hours > 12){
		hours -= 12;
	}
	return (int)hours;
}

//convert to binary

// void intToBin(int dec, int bin[], int numBits){
//     for(int i = 0; i < numBits; i++){
//         bin[i] = 1 & (dec >> i);
//     }
// }

void int_to_bin_digit(unsigned int in, int count, int* out)
{
    /* assert: count <= sizeof(int)*CHAR_BIT */
    unsigned int mask = 1U << (count-1);
    int i;
    for (i = 0; i < count; i++) {
        out[i] = (in & mask) ? 1 : 0;
        in <<= 1;
    }
}

void reverseArr(int arr[]){
	int t, end, c; 
	end = sizeof(arr) -1;
	for (c= 0; c< sizeof(arr); c++){
		t = arr[c];
		arr[c] = arr[end];
		arr[end]=t;	
	}
}
// array

void printArr(int arr[], int arrSize){
    for(int i = 0; i < arrSize; i++) {
        printf("%d ", arr[i]);
    }
}


/*
 * Turns on corresponding LED's for hours
 */
void lightHours(int units){
	// Write your logic to light up the hour LEDs here
	int ans[8];
	
	//intToBin(hFormat(hexCompensation(units)),ans,4);
	
	//reverseArr(ans);
    //int count = 0;
	printf("%d",units);
	int_to_bin_digit(units,8,ans);
	printArr(ans,8);
	digitalWrite(LEDS[0],ans[4]);
	digitalWrite(LEDS[1],ans[5]);
	digitalWrite(LEDS[2],ans[6]);
	digitalWrite(LEDS[3],ans[7]);


	// for(count; count<4; count++){
	// digitalWrite(LEDS[count],ans[count+4]);}
}

/*
 * Turn on the Minute LEDs
 */
void lightMins(int units){ 

	//printf("%d",units);
	int ans[8];
	int_to_bin_digit(units,8,ans);
	//intToBin(hexCompensation(units),ans,8);
	//reverseArr(ans);
	//printArr(ans,8);
	digitalWrite(LEDS[4],ans[2]);
	digitalWrite(LEDS[5],ans[3]);
	digitalWrite(LEDS[6],ans[4]);
	digitalWrite(LEDS[7],ans[5]);
	digitalWrite(LEDS[8],ans[6]);
	digitalWrite(LEDS[9],ans[7]);

    // int count = 0;
	// for(count; count<6; count++){
	// digitalWrite(LEDS[count+4],ans[count+2]);}
}

/*
 * PWM on the Seconds LED
 * The LED should have 60 brightness levels
 * The LED should be "off" at 0 seconds, and fully bright at 59 seconds
 */
void secPWM(int units){
	// max brightness at 1024,
	int intensity = (100/59)*units;
	softPwmWrite(SECS,intensity);


}

/*
 * hexCompensation
 * This function may not be necessary if you use bit-shifting rather than decimal checking for writing out time values
 */
int hexCompensation(int units){
	/*Convert HEX or BCD value to DEC where 0x45 == 0d45 
	  This was created as the lighXXX functions which determine what GPIO pin to set HIGH/LOW
	  perform operations which work in base10 and not base16 (incorrect logic) 
	*/
	int unitsU = units%0x10;

	if (units >= 0x50){
		units = 50 + unitsU;
	}
	else if (units >= 0x40){
		units = 40 + unitsU;
	}
	else if (units >= 0x30){
		units = 30 + unitsU;
	}
	else if (units >= 0x20){
		units = 20 + unitsU;
	}
	else if (units >= 0x10){
		units = 10 + unitsU;
	}
	return units;
}


/*
 * decCompensation
 * This function "undoes" hexCompensation in order to write the correct base 16 value through I2C
 */
int decCompensation(int units){
	int unitsU = units%10;

	if (units >= 50){
		units = 0x50 + unitsU;
	}
	else if (units >= 40){
		units = 0x40 + unitsU;
	}
	else if (units >= 30){
		units = 0x30 + unitsU;
	}
	else if (units >= 20){
		units = 0x20 + unitsU;
	}
	else if (units >= 10){
		units = 0x10 + unitsU;
	}
	return units;
}


/*
 * hourInc
 * Fetch the hour value off the RTC, increase it by 1, and write back
 * Be sure to cater for there only being 23 hours in a day
 * Software Debouncing should be used
 */
void hourInc(void){
	//Debounce
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
		printf("Interrupt 1 triggered, %d\n", hours);
		//Fetch RTC Time
		//hours = getHours();
	

		//Increase hours by 1, ensuring not to overflow
		if(hours < 23){
				hours+=1;
		}else {
			hours = 0;
		}
		//Write hours back to the RTC
		printf("I have: %d \n", hours);
		wiringPiI2CWriteReg8(RTC, HOUR, hours);
	}
	lastInterruptTime = interruptTime;
}

/* 
 * minInc
 * Fetch the minute value off the RTC, increase it by 1, and write back
 * Be sure to cater for there only being 60 minutes in an hour
 * Software Debouncing should be used
 */
void minInc(void){
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
		printf("Interrupt 2 triggered, %d\n", mins);

		//Fetch RTC Time
		// int mins = getMins();
	

		//Increase minutes by 1, ensuring not to overflow
		if(mins < 59){
				mins+=1;
		}else {
			mins = 0;
		}
		//Write minutes back to the RTC
		wiringPiI2CWriteReg8(RTC, HOUR, mins);
		
		
		
	}
	lastInterruptTime = interruptTime;
}

//This interrupt will fetch current time from another script and write it to the clock registers
//This functions will toggle a flag that is checked in main
void toggleTime(void){
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
		HH = getHours();
		MM = getMins();
		SS = getSecs();

		HH = hFormat(HH);
		HH = decCompensation(HH);
		wiringPiI2CWriteReg8(RTC, HOUR, HH);

		MM = decCompensation(MM);
		wiringPiI2CWriteReg8(RTC, MIN, MM);

		SS = decCompensation(SS);
		wiringPiI2CWriteReg8(RTC, SEC, 0b10000000+SS);

	}
	lastInterruptTime = interruptTime;
}
