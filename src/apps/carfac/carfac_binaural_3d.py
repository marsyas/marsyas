#!/usr/bin/python

import numpy as np
from enthought.mayavi import mlab
import marsyas
import sys

def marsyasplay(sfname):
  mng = marsyas.MarSystemManager()

  net = mng.create("Series","series")
  net.addMarSystem(mng.create("SoundFileSource", "src"))
  net.addMarSystem(mng.create("BinauralCARFAC", "carfac"))
  net.updControl("SoundFileSource/src/mrs_string/filename", marsyas.MarControlPtr.from_string(sfname))

  outData  = net.getControl("mrs_realvec/processedData")

  data = np.zeros((96,200,1))
  while net.getControl("SoundFileSource/src/mrs_bool/hasData").to_bool():
    net.tick()
    a = np.array(outData.to_realvec()).reshape((96,200,1))
    data = np.append(data,a,axis=2)

  obj = mlab.contour3d(data, contours=4, transparent=True)
  mlab.show()


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "Usage: test-marsyas-output-mfcc-data.py soundfile.wav"
        sys.exit(1)
    marsyasplay(sys.argv[1])
