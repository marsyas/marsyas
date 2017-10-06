


from bokeh.plotting import figure, show, output_file, vplot
from bokeh import palettes
from bokeh.layouts import column
from marsyas_util import create, control2array
import numpy as np
import json
import os
import math

def bokeh_plot(mydata, wav_fname): 
    # prepare some data
    y0 = mydata[0,:]
    y1 = mydata[1,:]
    zcrs = mydata[3,:] 
    rmss = mydata[2,:]
    x = np.array(range(0, len(y0)))
    zrs = np.zeros(len(y0))
    ones = np.ones(len(y0))
    
    rmss *= 255.0 / rmss.max() 
    

    # output to static HTML file
    output_file("lines.html")

    # palette 
    pal1 = palettes.all_palettes['Plasma'][256]
    pal2 = palettes.all_palettes['Greys'][256]

    colors1 = [] 
    for i in range(0, len(y0)): 
        colors1.append(pal1[int(rmss[i])])

    colors2 = [] 
    for i in range(0, len(y0)): 
        colors2.append(pal2[int(rmss[i])])

    start_angles = np.linspace(2*math.pi+math.pi/2, math.pi/2, len(y0))
    end_angles = np.linspace(2*math.pi + math.pi/2, math.pi/2, len(y0))
    end_angles += (2*math.pi /len(y0))

    # create a new plot with a title and axis labels
    p = figure(title=wav_fname, x_axis_label=None, y_axis_label=None, 
               plot_height=150, plot_width=800, toolbar_location=None)

    # add a line renderer with legend and line thickness
    #p.line(x,y0, legend="Line 0")
    #p.line(x,y1, legend="Line 1")


    # p.quad(top=zrs, left = x, bottom = -ones, right = x+1, color = colors1)
    # p.quad(top=ones, left = x, bottom = zrs, right = x+1, color = colors1)


    p.quad(top=zrs, left = x, bottom = y0, right = x+1, color = colors1)
    p.quad(top=y1, left = x, bottom = zrs, right = x+1, color = colors1)
#    p.wedge(x=0, y=0, radius = 0.5 * (y1-y0), start_angle = start_angles, end_angle = end_angles, color = colors1)

    


    return p 


def main(): 
    

    maxmin_spec = ["Series/mxm", 
              ["MaxMin/mxmin", 
               "Transposer/trs"]]
    
    spec = ["Series/plot", 
            ["SoundFileSource/src", 
             "Stereo2Mono/s2m",
             ["Fanout/measurements", 
             [
              "ZeroCrossings/zcrs",
              "Rms/rms",
              maxmin_spec
              ]]]]

    accum_spec = ["Accumulator/accum", 
                  [spec]]


    playlist_folder = "/Users/georgetzanetakis/data/sound/yogi_tunes/"
    print(playlist_folder)
    wav_fnames = [f for f in os.listdir(playlist_folder) if f.endswith(".wav")]

    wav_fnames = [playlist_folder + s for s in wav_fnames]

    print(wav_fnames)
    

#    wav_fnames = ["/Users/georgetzanetakis/data/sound/yogi_tunes/1 - The Player's Hands (Instrumental).wav", 
#                 "6_lion_heart.wav", 
#                  "10_alienist.wav"] 

    winSize = 44100
    plot_net = create(accum_spec)


    inSamples = plot_net.getControl("mrs_natural/inSamples")
    inSamples.setValue_natural(winSize)
    nTimes = plot_net.getControl("mrs_natural/nTimes")
    
    fname = plot_net.getControl("Series/plot/SoundFileSource/src/mrs_string/filename")


    figs = [] 
    for wav_fname in wav_fnames: 
        fname.setValue_string(wav_fname)
        fsize = plot_net.getControl("Series/plot/SoundFileSource/src/mrs_natural/size").to_natural()
        srate = plot_net.getControl("Series/plot/SoundFileSource/src/mrs_real/osrate").to_real()
        nTicks = int(fsize /winSize)
        print(("nTicks=", nTicks))
        nTimes.setValue_natural(nTicks)

        plot_net.tick() 
        mydata = control2array(plot_net, "mrs_realvec/processedData")    
        fig = bokeh_plot(mydata, wav_fname)
        figs.append(fig)
        
    show(column(*figs))
        


if __name__ == "__main__":
   main()

