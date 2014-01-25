
# Proof-of-concept implementation of the monophonic transcription
# system described by Bello, Monti and Sandler in ISMIR 2000.


import marsyas
import json
msm = marsyas.MarSystemManager()


# create a MarSystem from a recursive list specification
# def create(net):
#   composite = msm.create("Gain/id") # will be overwritten
#   if (len(net) == 2):
#     composite = msm.create(net[0])
#     msyslist = map(create,net[1])
#     msyslist = map(composite.addMarSystem,msyslist)
#   else:
#     composite = msm.create(net)
#   return composite

net = ["Series/net",
       ["SoundFileSource/src",
        ["Fanout/fanout",
         ["Gain/g1", "Gain/g2"]],
        "Gain/gain",
        "SoundFileSink/dest"]]

print json.dumps(net)
msys = create(net)
print msys.toStringShort()
