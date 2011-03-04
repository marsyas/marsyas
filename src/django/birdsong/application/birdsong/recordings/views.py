import subprocess
import re
import marsyas
import sys
import logging
from django.shortcuts import render_to_response
from django.http import HttpResponse

from recordings.models import Recording

#
# Pitch contour
#

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

# Calculate the pitch contour for the given annotation
def pitchcontour(request,annotation_id):
    # Find the annotation

    
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

#    return pitchcontour
#    return render_to_response('recordings/show.html',
    return render_to_response('recordings/pitchcontour.html',
                              {'pitches' : pitches,
                               'pitchcontour' : pitchcontour,
                               'median_filtered_pitchcontour' : median_filtered_pitchcontour,
                               'rmscontour' : rmscontour,
                               'numpitches' : len(pitches),
                               },
                              {}, mimetype="application/xhtml+xml")
    

#
# Views
#
def index(request):
    recordings = Recording.objects.all()
    return render_to_response('recordings/index.html', {'recordings' : recordings})

def show(request, recording_id):
    recording = Recording.objects.get(pk=int(request.POST.get('recording_id', 1)))
    annotations = recording.annotation_set.all()
    pitchcontours = ["100 220,105 217,111 220,117 220,122 213,128 220,134 220,139 220,145 220,151 210,157 209,162 220,168 217,174 217,179 218,185 220,191 220,197 217,202 210,208 218,214 218,219 220,225 220,231 217,237 220,242 215,248 196,254 192,259 202,265 199,271 198,277 174,282 172,288 171,294 170,299 169,305 169,311 169,317 169,322 169,328 169,334 169,339 169,345 169,351 169,357 170,362 170,368 171,374 171,379 171,385 172,391 172,397 172,402 171,408 172,414 170,419 172,425 172,431 172,437 172,442 172,448 196,454 173,459 172,465 173,471 170,477 170,482 172,488 195,494 169,499 196,505 195,511 171,516 195,522 195,528 171,534 172,539 171,545 220,551 195,556 194,562 195,568 195,574 195,579 212,585 195,591 217,596 195,602 212,608 217,614 196,619 213,625 215,631 172,636 220,642 217,648 215,654 217,659 171,665 210,671 217,676 213,682 196,688 220,694 195,699 215",
                     "100 320,105 317,111 320,117 320,122 313,128 320,134 320,139 320,145 320,151 310,157 309,162 320,168 317,174 317,179 318,185 320,191 320,197 317,202 310,208 318,214 318,219 320,225 320,231 317,237 320,242 315,248 196,254 192,259 302,265 199,271 198,277 174,282 172,288 171,294 170,299 169,305 169,311 169,317 169,322 169,328 169,334 169,339 169,345 169,351 169,357 170,362 170,368 171,374 171,379 171,385 172,391 172,397 172,402 171,408 172,414 170,419 172,425 172,431 172,437 172,442 172,448 196,454 173,459 172,465 173,471 170,477 170,482 172,488 195,494 169,499 196,505 195,511 171,516 195,522 195,528 171,534 172,539 171,545 320,551 195,556 194,562 195,568 195,574 195,579 312,585 195,591 317,596 195,602 312,608 317,614 196,619 313,625 315,631 172,636 320,642 317,648 315,654 317,659 171,665 310,671 317,676 313,682 196,688 320,694 195,699 315"]
    return render_to_response('recordings/show.html', {
            'annotations' : annotations,
            'recording_id' : recording_id,
            'pitchcontours' : pitchcontours
            }, {}, mimetype="application/xhtml+xml")

def show_annotations(request, recording_id, user_id):
    recording = Recording.objects.get(pk=int(request.POST.get('recording_id', 1)))
    annotations = recording.annotation_set.all()
    return render_to_response('recordings/show_annotations.html', {
            'annotations' : annotations,
            'recording_id' : recording_id})

def annotations(request, recording_id):
    recording = Recording.objects.get(pk=int(request.POST.get('recording_id', 1)))
    annotations = recording.annotation_set.all()
    return render_to_response('recordings/annotations.txt', {'annotations' : annotations})




