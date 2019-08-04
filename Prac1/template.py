#!/usr/bin/python3
"""
Names: Bongani Khoza
Student Number: KHZBON012
Prac: 1
Date: 23/07/2019
"""

# import Relevant Librares
import RPi.GPIO as GPIO
import time

#counter
counter = 0

def down(a):
    global counter
    
    if (counter > 0):
        counter -= 1
            
    else:
        counter = 7
        
    bin_value = list(bin(counter)[2:].zfill(3))
    GPIO.output(7, eval(bin_value[0]))
    GPIO.output(11, eval(bin_value[1]))
    GPIO.output(13, eval(bin_value[2]))
    

def up(a):
    
    global counter
    
    if (counter < 7):
        counter += 1
            
    else:
        counter = 0
        
    bin_value = list(bin(counter)[2:].zfill(3))
    
        
    GPIO.output(7, eval(bin_value[0]))
    GPIO.output(11, eval(bin_value[1]))
    GPIO.output(13, eval(bin_value[2]))


# Logic that you write
def main():
    
    #global counter
    global counter
    
    # Using board pinouts
    GPIO.setmode(GPIO.BOARD)

    # Set output pins (LEDs)
    GPIO.setup(7,GPIO.OUT)
    GPIO.setup(11,GPIO.OUT)
    GPIO.setup(13,GPIO.OUT)

    # Set input pins (switches)
    GPIO.setup(8 , GPIO.IN, GPIO.PUD_DOWN) #count up
    GPIO.setup(10, GPIO.IN, GPIO.PUD_DOWN) #count down
    
    

   
    GPIO.add_event_detect(8,GPIO.RISING, callback=up, bouncetime=300)
    GPIO.add_event_detect(10,GPIO.RISING, callback=down, bouncetime=300)

    while True:
        time.sleep(1)
            


# Only run the functions if 
if __name__ == "__main__":
    # Make sure the GPIO is stopped correctly
    try:
        while True:
           
            main()
    except KeyboardInterrupt:
        print("Exiting gracefully")
        # Turn off your GPIOs here
        GPIO.cleanup()
    # except e:
    #     GPIO.cleanup()
    #     print("Some other error occurred")
    #     print(e.message)