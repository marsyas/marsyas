# bextract implemented using the swig python Marsyas bindings
# George Tzanetakis, January, 16, 2007 

import marsyas_python 

# Create top-level patch
mng = marsyas_python.MarSystemManager()
fnet = mng.create("Series", "featureNetwork");

# functional short cuts to speed up typing 
create = mng.create
add = fnet.addMarSystem 
link = fnet.linkControl
upd = fnet.updControl
get  = fnet.getControl

# Add the MarSystems 
add(create("SoundFileSource", "src"));
add(create("TimbreFeatures", "featExtractor"));
add(create("TextureStats", "tStats")); 
add(create("Annotator", "annotator"));
add(create("WekaSink", "wsink"));

# link the controls to coordinate things
link("mrs_string/filename", "SoundFileSource/src/mrs_string/filename");
link("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");
link("WekaSink/wsink/mrs_string/currentlyPlaying","SoundFileSource/src/mrs_string/currentlyPlaying");
link("Annotator/annotator/mrs_natural/label", "SoundFileSource/src/mrs_natural/currentLabel");
link("SoundFileSource/src/mrs_natural/nLabels", "WekaSink/wsink/mrs_natural/nLabels");

# update controls to setup things 
upd("TimbreFeatures/featExtractor/mrs_string/disableTDChild", "all");
upd("TimbreFeatures/featExtractor/mrs_string/disableLPCChild", "all");
upd("TimbreFeatures/featExtractor/mrs_string/disableSPChild", "all");
upd("TimbreFeatures/featExtractor/mrs_string/enableSPChild", "MFCC/mfcc");
upd("mrs_string/filename", "bextract_single.mf");
upd("WekaSink/wsink/mrs_string/labelNames", 
			  get("SoundFileSource/src/mrs_string/labelNames"));
upd("WekaSink/wsink/mrs_string/filename", "bextract_python.arff");

# do the processing extracting MFCC features and writing to weka file 
previouslyPlaying = ""
while (get("mrs_bool/notEmpty")):
    currentlyPlaying = get("SoundFileSource/src/mrs_string/currentlyPlaying");
    if (currentlyPlaying != previouslyPlaying) :
	print "Processing: " +  currentlyPlaying 
    fnet.tick();
    previouslyPlaying = currentlyPlaying 















