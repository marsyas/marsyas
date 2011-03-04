from django.shortcuts import render_to_response
import commands
import subprocess
import re
import marsyas
import numpy as np
from django.http import HttpResponse
from django.core.servers.basehttp import FileWrapper
import time

# Redirect all output to stderr
import sys
sys.stdout = sys.stderr

def index(request):
    # Get the request parameters
    method = request.POST.get('method', 'bandpass')
    print "method=%s" % method

    frequency = float(request.POST.get('frequency', '500'))
    print "frequency=%f" % frequency

    qfactor = float(request.POST.get('qfactor', '500'))
    print "qfactor=%f" % qfactor

    gain = float(request.POST.get('gain', '0.8'))
    print "gain=%f" % gain

    start = float(request.POST.get('start', '15'))
    print "start=%f" % start

    length = float(request.POST.get('length', '30'))
    print "length=%f" % length

    # The temporary filename to create
    lt = time.localtime(time.time())
    output_filename = "/tmp/orchive_filter_%02d_%02d_%04d_%02d_%02d_%02d.wav" % (lt[2], lt[1], lt[0], lt[3], lt[4], lt[5])
    print output_filename
    
    # Generate the file
    #input_filename = "/Users/sness/venus/assets/recordings/1007/H2/2010/01/01/1/audio.wav"
    #input_filename = "/home/sness/wavfiles/tiny.wav"
    #input_filename = "/home/sness/nDEV/venus_orchive-assets/recordings/1007/H2/2010/01/01/1/audio.wav"
    input_filename = "/home/sness/wavfiles/audio.wav"

    print "Generating file in=%s out=%s" % (input_filename, output_filename)
    command = "/home/sness/marsyas/release/bin/sound2sound %s -m bandpass -f %f -q %f -s %f -l %f -g %f %s" % (input_filename,frequency, qfactor, start, length, gain, output_filename)
    print "command=%s" % command
    a = commands.getoutput(command)
    print a

    # Send the file to the user
    print "Sending file to user"
    sendfile = open(output_filename, "r")
    response = HttpResponse(FileWrapper(sendfile), content_type='audio/x-wav')
    response['Content-Disposition'] = 'attachment; filename=file.wav'
    return response
