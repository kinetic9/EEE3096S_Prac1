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

# Logic that you write
def main():
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(7,GPIO.OUT)

    GPIO.output(7, GPIO.LOW)
    print("LED Off")
    time.sleep(2)

    GPIO.output(7, GPIO.HIGH)
    print("LED On")
    time.sleep(2)



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
