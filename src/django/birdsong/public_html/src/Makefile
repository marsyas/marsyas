all: ../orcaannotator.swf

../orcaannotator.swf: OrcaAnnotator.hx Slider.hx PushButton.hx ControlBar.hx PushButtonEvent.hx VolumeButton.hx VolumeSlider.hx TimeIndicator.hx SoundPlayer.hx ProgressBar.hx ProgressBarReticle.hx Annotator.hx Annotation.hx AnnotationExtent.hx XMLLoader.hx GlobalSettings.hx Prediction.hx orcaannotator.hxml swflib.swf
	export HAXE_LIBRARY_PATH=/usr/lib/haxe/std:. && haxe orcaannotator.hxml

swflib.swf: swflib.xml
	/usr/sness/haxe/bin/swfmill simple swflib.xml swflib.swf
