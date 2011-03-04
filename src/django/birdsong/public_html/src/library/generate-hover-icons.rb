#!/usr/bin/ruby

@icons = [
          "ffwd",
          "ffwdend",
          "layer_delete",
          "layer_down",
          "layer_up",
          "pane_delete",
          "playpause",
          "rewind",
          "rewindstart",
          "zoomin",
          "zoomout",
          "zoomselection",
          "zoomfull",
          "save",
          "annotation",
          "info",
          "prev_annotation",
          "next_annotation",
          "follow"
         ]
         
@icons.each do |n|
  puts "Converting #{n}"
  `convert #{n}.png -fill white -colorize 50% #{n}-disabled.png`
  `convert #{n}.png -fill white -colorize 60% #{n}-over.png`
  `convert #{n}.png -negate #{n}-active.png`
end

#
# Copy the pretty handy customized orange save active button
#
`cp save-active-follow.png follow-active.png`
`cp save-active-save.png save-active.png`
