#!/usr/bin/python

import osc

def myTest():
    """ a simple function that creates the necesary sockets and enters an enless
        loop sending and receiving OSC
    """
    osc.init()
    osc.bind(printStuff, "/gainSlider")
    inSocket = osc.createListener("127.0.0.1", 9001)

    import time

    while 1:
        osc.getOSC(inSocket)

    osc.dontListen()

def printStuff(*msg):
    """deals with "print" tagged OSC addresses """

    print "printing in the printStuff function ", msg
    print "the oscaddress is ", msg[0][0]
    print "the value is ", msg[0][2]


if __name__ == '__main__': myTest()
