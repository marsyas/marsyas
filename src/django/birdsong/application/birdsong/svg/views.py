from django.shortcuts import render_to_response
import subprocess
import re
import marsyas

def index(request):
    out = subprocess.Popen('/home/sness/marsyas/release/bin/sound2png -json /home/sness/wavfiles/tiny.wav', shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

    points = ""
    for line in out.stdout.readlines():
        points += line

    return render_to_response('svg/index.html', {'points' : points})
                              
