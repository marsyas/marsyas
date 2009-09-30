#!/usr/bin/python

import osc

def myTest():
    """ a simple function that creates the necesary sockets and enters an enless
        loop sending and receiving OSC
    """
    osc.init()
    osc.listen('127.0.0.1', 10001)
    osc.bind(printStuff, "/generickey")
    osc.bind(printStuff, "/key1")
    osc.bind(printStuff, "/key2")
    osc.bind(printStuff, "/key3")
    inSocket = osc.createListener("127.0.0.1", 9000)

    import time

    while 1:
        osc.getOSC(inSocket)

    osc.dontListen()

def printStuff(*msg):
    """deals with "print" tagged OSC addresses """

    print msg

if __name__ == '__main__': myTest()














