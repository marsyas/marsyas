from django.shortcuts import render_to_response
import commands
import subprocess
import re
import marsyas
import numpy as np
from django.http import HttpResponse
from django.core.servers.basehttp import FileWrapper
import time

def index(request):
    # Get the request parameters
    window_size = float(request.POST.get('window_size', '8192'))
    print "window_size=%f" % window_size

    hop_size = float(request.POST.get('hop_size', '8192'))
    print "hop_size=%f" % hop_size

    gain = float(request.POST.get('gain', '64'))
    print "gain=%f" % gain

    max_frequency = float(request.POST.get('max_frequency', '8000'))
    print "max_frequency=%f" % max_frequency

    start = float(request.POST.get('start', '15'))
    print "start=%f" % start

    length = float(request.POST.get('length', '30'))
    print "length=%f" % length
    
    # The temporary filename to create
    lt = time.localtime(time.time())
    output_filename = "/tmp/orchive_spectrogram_%02d_%02d_%04d_%02d_%02d_%02d.png" % (lt[2], lt[1], lt[0], lt[3], lt[4], lt[5])
    print output_filename
    
    # Generate the file
    #input_filename = "/home/sness/nDEV/venus_orchive/public/assets/recordings/1007/H2/2010/01/01/1/audio.wav"
    #input_filename = "/home/sness/wavfiles/tiny.wav"
    #input_filename = "/home/sness/nDEV/venus_orchive-assets/recordings/1007/H2/2010/01/01/1/audio.wav"
    input_filename = "/home/sness/wavfiles/audio.wav"

    print "Generating file in=%s out=%s" % (input_filename, output_filename)
    command = "/home/sness/marsyas/release/bin/sound2png %s -m neptune -ws %f -hs %f -g %f -mf %f -s %f -l %f %s" % (input_filename, window_size, hop_size, gain, max_frequency, start, length, output_filename)
    print "command=%s" % command
    a = commands.getoutput(command)
    print a

    # Send the file to the user
    print "Sending file to user"
    sendfile = open(output_filename, "r")
    response = HttpResponse(FileWrapper(sendfile), content_type='image/png')
    response['Content-Disposition'] = 'attachment; filename=file.png'
    return response
