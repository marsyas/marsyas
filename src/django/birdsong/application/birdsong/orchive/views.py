from django.shortcuts import render_to_response
import subprocess
import re
import marsyas
import numpy as np

# Redirect all output to stderr
import sys
sys.stdout = sys.stderr

def index(request):
    buffer_size = int(request.GET.get('buffer_size', '1024'))
    print "buffer_size=%i" % buffer_size

    hop_size = int(request.GET.get('hop_size', '1024'))
    print "hop_size=%i" % hop_size

    #sfname = "/Users/sness/venus/assets/recordings/1007/H2/2010/01/01/1/audio.wav"
    #sfname = "/home/sness/wavfiles/tiny.wav"
    #sfname = "/home/sness/nDEV/venus_orchive-assets/recordings/1007/H2/2010/01/01/1/audio.wav"
    sfname = "/home/sness/wavfiles/audio.wav"
    print "sfname=%s" % sfname
    mng = marsyas.MarSystemManager()

    # Create net
    net = mng.create("Series","series")

    # Make a SoundFileSource and convert it to mono
    net.addMarSystem(mng.create("SoundFileSource", "src"));
    net.addMarSystem(mng.create("Stereo2Mono", "s2m"));

    # A fanout that will do both RMS and Flux calculations
    fanout = mng.create("Fanout","fanout");
    net.addMarSystem(fanout);

    # The branch to do the RMS
    rms_series = mng.create("Series","rms_series");
    rms_series.addMarSystem(mng.create("Rms", "rms"));
    fanout.addMarSystem(rms_series);

    # The branch to do the Flux
    flux_series = mng.create("Series","flux_series");
    flux_series.addMarSystem(mng.create("ShiftInput", "si"));
    flux_series.addMarSystem(mng.create("Windowing", "win"));
    flux_series.addMarSystem(mng.create("Spectrum","spk"));
    flux_series.addMarSystem(mng.create("PowerSpectrum", "pspk"));
    flux_series.addMarSystem(mng.create("Flux", "flux")); 
    fanout.addMarSystem(flux_series);

    # Update the controls with required values
    net.updControl("SoundFileSource/src/mrs_string/filename", marsyas.MarControlPtr.from_string(sfname))

    print "############################## la ##############################"

    rms_python_array = []
    flux_python_array = []
    while net.getControl("SoundFileSource/src/mrs_bool/hasData").to_bool():
        data = net.getControl("mrs_realvec/processedData").to_realvec()
        rms = data[0]
        flux = data[1]
        rms_python_array.append(rms)
        flux_python_array.append(flux)
        net.tick()

    # Convert these arrays to numpy vectors
    rms_array = np.float32(rms_python_array)
    flux_array = np.float32(flux_python_array)

    # Normalize these arrays
    rms_array *= 1.0/rms_array.max()
    flux_array *= 1.0/flux_array.max()
    print rms_array
    print flux_array

    # Calculate the RGBA values
    rgba = []
    for i in range(0,len(rms_array)):
        # The RMS and Flux values
        rms = rms_array[i]
        flux = flux_array[i]

        # Alpha is just the RMS value
        alpha = rms * 0.5

        # Map the flux to a mix of red and green
        red = flux
        green = 1.0 - flux
        blue = 0

        # Add these values as a tuple to the array rgba
        rgba.append((red,green,blue,alpha))

    output = ""
    hop_width_ms = int((hop_size / 44100.0) * 1000)
    for i in range(0, len(rgba)):
        start_time_ms = int(((hop_size / 44100.0) * float(i))*1000)
        end_time_ms = start_time_ms + hop_width_ms
        red_int = int(rgba[i][0] * 255)
        green_int = int(rgba[i][1] * 255)
        blue_int = int(rgba[i][2] * 255)
        color = ('%02x%02x%02x' % (red_int, green_int, blue_int)).upper()
        output += "%i,%i,%i,,%f,0x%s\n" % (i, start_time_ms, end_time_ms, rgba[i][3], color)

    return render_to_response('orchive/index.html', {'output':output}, {})
