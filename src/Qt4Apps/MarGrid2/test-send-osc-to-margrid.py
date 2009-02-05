#!/usr/bin/python

import osc

def myTest():
    """ a simple function that creates the necesary sockets and enters an endless
        loop sending and receiving OSC
    """
    osc.init()

    import time

    i = 0
    while 1:
        i = i % 4
        print i
        osc.sendMsg("/x0", [i], "127.0.0.1", 9005)
        osc.sendMsg("/y0", [i+1], "127.0.0.1", 9005)

        osc.sendMsg("/x1", [i+2], "127.0.0.1", 9005)
        osc.sendMsg("/y1", [i+3], "127.0.0.1", 9005)

        osc.sendMsg("/update", [1], "127.0.0.1", 9005)

        bundle = osc.createBundle()
        time.sleep(1)
        i += 1

    osc.dontListen()


if __name__ == '__main__': myTest()














