#!/usr/bin/python

import osc

def myTest():
    """ a simple function that creates the necesary sockets and enters an endless
        loop sending and receiving OSC
    """
    osc.init()

    import time

    i = 1
    while 1:
        j = ((i + 1) % 2) * 100
        print j
        osc.sendMsg("/gainSlider", [j], "127.0.0.1", 9000)
        bundle = osc.createBundle()
        time.sleep(1)
        i += 1

    osc.dontListen()


if __name__ == '__main__': myTest()














