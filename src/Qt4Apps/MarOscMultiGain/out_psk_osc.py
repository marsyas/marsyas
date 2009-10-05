#!/usr/bin/python

import osc
import serial
import re

def myTest():
    """ a simple function that creates the necesary sockets and enters an enless
        loop sending and receiving OSC
    """
    osc.init()

    import time
    
    # Setup the keyboard
    ser = serial.Serial(port='/dev/tty.usbserial-A6007kFY',baudrate= 115200,bytesize=8,parity='N',stopbits=1)
    ser.open()
    print ser.portstr
    ser.write("q") #initiate continuous ASCII mode (active keys only)

    # Build a regexp to recognize a line that looks like:
    #
    # (0,10) => 158
    #
    regexp = re.compile('.*\(([0-9]*),([0-9]*)\) => ([0-9]*)')

    # Main loop
    i = 1
    while 1:
	line = ser.readline()
	r = regexp.match(line)
	if r is not None:
		#print "****" + r.group(1) + " " + r.group(2) + " " + r.group(3) +  "****"
                row = int(r.group(1))
                col = int(r.group(2))
                pressure = int(r.group(3)) / 10
                address = "/generickey"

	


                if (row == 1) and (col == 9):
                    address = "/key1"
                if row == 5 and col == 8:
                    address = "/key2"
                if row == 0 and col == 6:
                    address = "/key3"
		if row == 5 and col == 9: 
		    address = "/key4" 
		if row == 4 and col == 8: 
		    address = "/key5" 
		if row == 1 and col == 6: 
		    address = "/key6"
		if row == 4 and col == 9: 
		    address = "/key7"
		if row == 3 and col == 8: 
		    address = "/key8" 
		if row == 4 and col == 6: 
		    address = "/key9" 
		if row == 1 and col == 11: 
		    address = "/keym" 
		if row == 5 and col == 3: 
		    address = "/keyd" 
		if row == 5 and col == 2: 
		    address = "/keys"
		if row == 0 and col == 17: 
		    address = "/keyleft"		
		if row == 0 and col == 18: 
		    address = "/keydown"		
		if row == 0 and col == 10: 
		    address = "/keyright"		
		if row == 1 and col == 18: 
		    address = "/keyup"		

                if pressure < 20:
                    pressure = 0
		pressure /= 85.0;

		if address == "/key1": 
		   address = "/Fanout/mixsrc/Series/branch1/Panorama/pan/mrs_real/angle" 
		   pressure *= -0.785
		if address == "/key2": 
		   address = "/Fanout/mixsrc/Series/branch1/Panorama/pan/mrs_real/angle" 
		   pressure = 0
		if address == "/key3": 
		   address = "/Fanout/mixsrc/Series/branch1/Panorama/pan/mrs_real/angle" 
		   pressure *= 0.785;

		if address == "/key4": 
		   address = "/Fanout/mixsrc/Series/branch2/Panorama/pan/mrs_real/angle" 
		   pressure *= -0.785
		if address == "/key5": 
		   address = "/Fanout/mixsrc/Series/branch2/Panorama/pan/mrs_real/angle" 
		   pressure = 0
		if address == "/key6": 
		   address = "/Fanout/mixsrc/Series/branch2/Panorama/pan/mrs_real/angle" 
		   pressure *= 0.785;


		if address == "/key7": 
		   address = "/Fanout/mixsrc/Series/branch3/Panorama/pan/mrs_real/angle" 
		   pressure *= -0.785
		if address == "/key8": 
		   address = "/Fanout/mixsrc/Series/branch3/Panorama/pan/mrs_real/angle" 
		   pressure = 0
		if address == "/key9": 
		   address = "/Fanout/mixsrc/Series/branch3/Panorama/pan/mrs_real/angle" 
		   pressure *= 0.785;

		if address == "/keym": 
		   pressure *= 100;
		if address == "/keys": 
		   pressure *= 100;
		if address == "/keyd":
		   pressure *= 20;


		if address == "/keyleft": 
		   pressure *= -255;
		if address == "/keyright":
		   pressure *= 255;
		if address == "/keyup": 
		   pressure *= 180;
		if address == "/keydown":
		   pressure *= -180;

                print "**** address=" + address + " row=" + str(row) + " col=" + str(col) + " pressure=" + str(pressure) + " ****"

                osc.sendMsg(address, [pressure], "127.0.0.1", 9000)
#         bundle = osc.createBundle()
        #time.sleep(0.5)
        i += 1

    osc.dontListen()
    ser.close()


if __name__ == '__main__': myTest()


