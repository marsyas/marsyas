#!/usr/bin/env python

from pylab import *
from marsyas import MarSystemManager,MarControlPtr

msm = MarSystemManager()

weka= msm.create("WekaSource", "weka")
classifier= msm.create("GaussianClassifier", "classifier")
summary= msm.create("Summary", "summary")
pipe = msm.create("Series","pipe")

pipe.addMarSystem(weka)
pipe.addMarSystem(classifier)
pipe.addMarSystem(summary)

filename = pipe.getControl("WekaSource/weka/mrs_string/filename")
validationmode = pipe.getControl("WekaSource/weka/mrs_string/validationMode")


validationmode.setValue_string("PercentageSplit,50%")
filename.setValue_string("ms.arff")

insamples= pipe.getControl("mrs_natural/inSamples")
insamples.setValue_natural(1)

wekanclasses= pipe.getControl("WekaSource/weka/mrs_natural/nClasses")
summarynclasses= pipe.getControl("Summary/summary/mrs_natural/nClasses")
classifiernclasses= pipe.getControl("GaussianClassifier/classifier/mrs_natural/nClasses")
summarynclasses.setValue_natural(wekanclasses.to_natural());
classifiernclasses.setValue_natural(wekanclasses.to_natural())

wekaclassnames= pipe.getControl("WekaSource/weka/mrs_string/classNames")
summaryclassnames= pipe.getControl("Summary/summary/mrs_string/classNames")
summaryclassnames.setValue_string( wekaclassnames.to_string())


isdone= pipe.getControl("WekaSource/weka/mrs_bool/done")
done= pipe.getControl("Summary/summary/mrs_bool/done")
modectl= pipe.getControl("WekaSource/weka/mrs_string/mode")
modectlsummary= pipe.getControl("Summary/summary/mrs_string/mode")

wekaprocessed= pipe.getControl("WekaSource/weka/mrs_realvec/processedData")


pipe.linkControl("GaussianClassifier/classifier/mrs_string/mode","Summary/summary/mrs_string/mode")
modectlsummary.setValue_string(modectl.to_string()) 


print validationmode.to_string()
while isdone.to_bool() == False:
	mode = modectl.to_string() 
	print mode
	pipe.tick()
	modectlsummary.setValue_string(mode) 
	i = 0
#	mysample = wekaprocessed.to_realvec() 	
#	while i < len(mysample):
#		print mysample[i],
#		i = i+1




modectlsummary.setValue_string("predict") 
done.setValue_bool(True)
pipe.tick()
