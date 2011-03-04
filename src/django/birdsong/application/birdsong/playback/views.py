from django.shortcuts import render_to_response
import subprocess
import re
import marsyas

def index(request):
    return render_to_response('playback/index.html',
                              {},
                              {}, mimetype="application/xhtml+xml")
                              
