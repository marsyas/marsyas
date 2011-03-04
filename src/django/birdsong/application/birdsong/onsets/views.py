from django.template import Context, loader
from django.http import HttpResponse
from django.http import Http404
from django.shortcuts import get_object_or_404, render_to_response
from calls.onsets.models import Recording
import subprocess
import re

# Redirect all output to stderr
import sys
sys.stdout = sys.stderr

def index(request, recording_id):
    recording = get_object_or_404(Recording, pk=recording_id)
    onsets = []

    input_filename = "/home/sness/wavfiles/2005_449A_trim.wav"
    
    if request.method == 'POST':
        p = subprocess.Popen('/usr/sness/marsyas/bin/onsets %s' % (input_filename),
                             shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        # Read the output lines
        for line in p.stdout.readlines():
            print "line=%s" % line
            m = re.search('^([0-9.]+)$', line.strip())
            if m is not None:
                onsets.append(float(m.group(1)))

    onset_pixels = []
    image_width = 837
    audio_length = 30.0
    for i in range(0,len(onsets)):
        onset_pixels.append((onsets[i] / audio_length) * image_width)
                
    return render_to_response('onsets/index.html',
                              {'onsets' : onsets, 'onset_pixels' : onset_pixels, 'recording' : recording},
                              {}, mimetype="application/xhtml+xml")


