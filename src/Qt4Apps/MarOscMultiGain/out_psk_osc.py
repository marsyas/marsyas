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
    ser = serial.Serial(port='/dev/ttyUSB0',baudrate= 115200,bytesize=8,parity='N',stopbits=1)
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

                if pressure < 20:
                    pressure = 0

                print "**** address=" + address + " row=" + str(row) + " col=" + str(col) + " pressure=" + str(pressure) + " ****"

                osc.sendMsg(address, [pressure], "127.0.0.1", 9000)
#         bundle = osc.createBundle()
        #time.sleep(0.5)
        i += 1

    osc.dontListen()
    ser.close()


if __name__ == '__main__': myTest()


