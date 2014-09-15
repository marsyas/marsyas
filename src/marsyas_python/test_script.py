
import marsyas 
import json 


net = marsyas.system_from_script_file("test.mrs")
print net.toStringShort()


net = marsyas.system_from_script(""" 

     Network : Series
     {
        -> input: NoiseSource
        -> amp: Gain
        -> output: AudioSink
     }
""")
print net.toStringShort()


