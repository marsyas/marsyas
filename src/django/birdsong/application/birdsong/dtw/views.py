from django.shortcuts import render_to_response
import subprocess
import re
import marsyas
import sys

def median_filter(input,window_width):

    half = window_width / 2
    output = []
    for i in range(half,len(input)-half):
        b = []
        for j in range(i-half,i+half+1):
            b.append(input[j])
            c = set(b)
            max_count = 0
            max_val = -1
            for n in c:
                if b.count(n) > max_count:
                    max_count = b.count(n)
                    max_val = n
        output.append(max_val)
    return output

def index(request):
    return render_to_response('dtw/index.html',
                              {},
                              {}, mimetype="application/xhtml+xml")


def pitchcontour(request,median):
    pitches = []
    output = ""

    p = subprocess.Popen('/usr/sness/marsyas/bin/pitchextract -y /home/sness/wavfiles/A36_N4__2_.wav',
                         shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    # Read the output lines
    for line in p.stdout.readlines():
        output += line
        m = re.search('^([0-9.]+)$', line.strip())
        if m is not None:
            pitches.append(float(m.group(1)))

    # Generate a pretty SVG string for the pitch contour
    pitchcontour = ""
    for i in range(0,len(pitches)):
        if (pitches[i] < 5000):
            pitchcontour += "%i %i" % (i * 2, 200.0 - pitches[i] / 20.0)
            if i < (len(pitches) - 1):
                pitchcontour += ","

    median_filtered_pitches = median_filter(pitches,int(median))

    # Generate a pretty SVG string for the median filtered pitch contour
    median_filtered_pitchcontour = ""
    for i in range(0,len(median_filtered_pitches)):
        if (median_filtered_pitches[i] < 5000):
            median_filtered_pitchcontour += "%i %i" % (i * 2, 200.0 - median_filtered_pitches[i] / 20.0)
            if i < (len(median_filtered_pitches) - 1):
                median_filtered_pitchcontour += ","

    return render_to_response('dtw/pitchcontour.html',
                              {'pitches' : pitches, 'output' : output,
                               'pitchcontour' : pitchcontour,
                               'median_filtered_pitchcontour' : median_filtered_pitchcontour,
                               },
                              {}, mimetype="application/xhtml+xml")

def pitchcontour_embed(request):
    # Redirect all output to stderr
    sys.stdout = sys.stderr

    pitches = []
    rmses = []
    output = ""
    data = ""

    buffer_size = int(request.GET.get('buffer_size', '1024'))
    print "buffer_size=%i" % buffer_size

    hop_size = int(request.GET.get('hop_size', '1024'))
    print "hop_size=%i" % hop_size

    tolerance = float(request.GET.get('tolerance', '0.7'))
    print "tolerance=%i" % tolerance

    median = float(request.GET.get('median', '11'))
    print "median=%i" % median

    #sfname = "/home/sness/wavfiles/tiny.wav"
    sfname = "/home/sness/wavfiles/A36_N4__2_.wav"
    print "sfname=%s" % sfname
    mng = marsyas.MarSystemManager()
    print "la"

    # Create net
    net = mng.create("Series","series")

    net.addMarSystem(mng.create("SoundFileSource", "src"))

    # Create fanout
    fanout = mng.create("Fanout","fanout")

    # Create the Yin series
    yin_series = mng.create("Series","yin_series")
    yin_series.addMarSystem(mng.create("ShiftInput", "si"));
    yin_series.addMarSystem(mng.create("Yin", "yin"));
    yin_series.addMarSystem(mng.create("Gain", "gain"));

    # Create the Yin series
    rms_series = mng.create("Series","rms_series")
    rms_series.addMarSystem(mng.create("Rms", "rms"));
    rms_series.addMarSystem(mng.create("Gain", "gain"));

    # Add these system to the main network
    fanout.addMarSystem(yin_series)
    fanout.addMarSystem(rms_series)
    net.addMarSystem(fanout)

    net.updControl("mrs_natural/inSamples", hop_size)
    net.updControl("SoundFileSource/src/mrs_string/filename", marsyas.MarControlPtr.from_string(sfname))

    net.updControl("Fanout/fanout/Series/yin_series/ShiftInput/si/mrs_natural/winSize", buffer_size);
    yin_series.updControl("Yin/yin/mrs_natural/inSamples",buffer_size);
    yin_series.updControl("Yin/yin/mrs_real/tolerance",tolerance);

    max_rms = 0
    while net.getControl("SoundFileSource/src/mrs_bool/hasData").to_bool():
        pitch = net.getControl("Fanout/fanout/Series/yin_series/Yin/yin/mrs_realvec/processedData").to_realvec()
        pitches.append(pitch[0])
        rms = net.getControl("Fanout/fanout/Series/rms_series/Rms/rms/mrs_realvec/processedData").to_realvec()
        rmses.append(rms[0])
        if (rms[0] > max_rms):
            max_rms = rms[0]
        net.tick()

    print "pitches="
    print pitches
    print "rmses="
    print rmses

    sound_file_len = 2.439
    width = 600.
    x_offset = 100
    y_offset = 20

    # Generate a pretty SVG string for the pitch contour
    pitchcontour = ""
    for i in range(0,len(pitches)):
        if (pitches[i] < 5000):  # filter out those really high pitches that YIN gives
            time = int(((hop_size / 44100.0) * float(i)) / sound_file_len * width) + x_offset
            pitchcontour += "%i %i" % (time, (200.0 - pitches[i] / 50.0) + y_offset)
            if i < (len(pitches) - 1):
                pitchcontour += ","

    median_filtered_pitches = []
    median_filtered_pitches = median_filter(pitches,int(median))
    #print median_filtered_pitches

     # Generate a pretty SVG string for the median filtered pitch contour
    median_filtered_pitchcontour = ""
    for i in range(0,len(median_filtered_pitches)):
        if (median_filtered_pitches[i] < 5000):
            time = int(((hop_size / 44100.0) * float(i)) / sound_file_len * width) + x_offset
            median_filtered_pitchcontour += "%i %i" % (time, (200.0 - median_filtered_pitches[i] / 50.0) + y_offset)
 #            median_filtered_pitchcontour += "%i %i" % (i * 2, 200.0 - median_filtered_pitches[i] / 20.0)
            if i < (len(median_filtered_pitches) - 1):
                median_filtered_pitchcontour += ","

    # Generate a pretty SVG string for the rms contour
    rmscontour = ""
    for i in range(0,len(rmses)):
        if (rmses[i] < 5000):  # filter out those really high rmses that YIN gives
            time = int(((hop_size / 44100.0) * float(i)) / sound_file_len * width) + x_offset
            rmscontour += "%i %i" % (time, 200 - ((rmses[i] / max_rms) * 200.))
            if i < (len(rmses) - 1):
                rmscontour += ","

    return render_to_response('dtw/pitchcontour-embed.html',
                              {'pitches' : pitches,
                               'pitchcontour' : pitchcontour,
                               'median_filtered_pitchcontour' : median_filtered_pitchcontour,
                               'rmscontour' : rmscontour,
                               'numpitches' : len(pitches),
                               },
                              {}, mimetype="application/xhtml+xml")
