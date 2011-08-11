#!/usr/bin/python

#import numpy
#from mayavi.mlab import *

import numpy as np
from enthought.mayavi import mlab

# x, y, z = np.ogrid[-10:10:20j, -10:10:20j, -10:10:20j]
# s = np.sin(x*y*z)/(x*y*z)

# src = mlab.pipeline.scalar_field(s)
# mlab.pipeline.iso_surface(src, contours=[s.min()+0.1*s.ptp(), ], opacity=0.3)
# mlab.pipeline.iso_surface(src, contours=[s.max()-0.1*s.ptp(), ],)

# mlab.show()

# def test_contour3d():

# x, y, z = np.ogrid[-5:5:64j, -5:5:64j, -5:5:64j]
# scalars = x*x*0.5 + y*y + z*z*2.0
# print scalars
scalars = [[[0,0,0],[1,0,1],[2,0,2]]]
obj = mlab.contour3d(scalars, contours=4, transparent=True)
mlab.show()

#   return obj

# test_contour3d()
