import numpy

### from:
# http://www.johnvinyard.com/blog/?p=268
def norm_shape(shape):
    '''
    Normalize numpy array shapes so they're always expressed as a
tuple,
    even for one-dimensional shapes.
     
    Parameters
        shape - an int, or a tuple of ints
     
    Returns
        a shape tuple
    '''
    try:
        i = int(shape)
        return (i,)
    except TypeError:
        # shape was not a number
        pass
 
    try:
        t = tuple(shape)
        return t
    except TypeError:
        # shape was not iterable
        pass
     
    raise TypeError('shape must be an int, or a tuple of ints')

def sliding_window(a,ws,ss = None,flatten = True):
    '''
    Return a sliding window over a in any number of dimensions
     
    Parameters:
        a  - an n-dimensional numpy array
        ws - an int (a is 1D) or tuple (a is 2D or greater)
representing the size
             of each dimension of the window
        ss - an int (a is 1D) or tuple (a is 2D or greater)
representing the
             amount to slide the window in each dimension. If not
specified, it
             defaults to ws.
        flatten - if True, all slices are flattened, otherwise,
there is an
                  extra dimension for each dimension of the inumpyut.
     
    Returns
        an array containing each n-dimensional window from a
    '''
     
    if None is ss:
        # ss was not provided. the windows will not overlap in any
        # direction.
        ss = ws
    ws = norm_shape(ws)
    ss = norm_shape(ss)
     
    # convert ws, ss, and a.shape to numpy arrays so that we can
    # do math in every
    # dimension at once.
    ws = numpy.array(ws)
    ss = numpy.array(ss)
    shape = numpy.array(a.shape)
     
     
    # ensure that ws, ss, and a.shape all have the same number of
    # dimensions
    ls = [len(shape),len(ws),len(ss)]
    if 1 != len(set(ls)):
        raise ValueError('a.shape, ws and ss must all have the same length. The were %s' % str(ls))
     
    # ensure that ws is smaller than a in every dimension
    if numpy.any(ws > shape):
        raise ValueError('ws cannot be larger than a in any dimension. a.shape was %s and ws was %s' % (str(a.shape),str(ws)))
     
    # how many slices will there be in each dimension?
    newshape = norm_shape(((shape - ws) // ss) + 1)
    # the shape of the strided array will be the number of slices
    # in each dimension
    # plus the shape of the window (tuple addition)
    newshape += norm_shape(ws)
    # the strides tuple will be the array's strides multiplied by
    # step size, plus
    # the array's strides (tuple addition)
    newstrides = norm_shape(numpy.array(a.strides) * ss) + a.strides
    strided = numpy.lib.stride_tricks.as_strided(a,shape = newshape,strides = newstrides)
    if not flatten:
        return strided
     
    # Collapse strided so that it has one more dimension than the
    # window.  I.e.,
    # the new array is a flat list of slices.
    meat = len(ws) if ws.shape else 0
    firstdim = (numpy.product(newshape[:-meat]),) if ws.shape else ()
    dim = firstdim + (newshape[-meat:])
    # remove any dimensions with size 1
    #dim = filter(lambda i : i != 1,dim)
    return strided.reshape(dim)


