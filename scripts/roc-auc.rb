#!/usr/bin/ruby

#
# This program builds the ROC curve for predict.txt at 
# different thresholding levels.  It then calculates
# the AUC of this curve. 
#
# Pseudocode:
#
# 1) Read in ground truth and put the data into a matrix.  This is
#    binary data.
#
# 2) Read in prediction file and put the data into a matrix.  This is
#    affinity data.
#
# 3) Loop over the increments we want to search over and for each
#    increment, loop over all the elements in the prediction file
#    matrix.  If the value in the matrix is less than the cutoff, put
#    a 0 in the output matrix, if it is more, then put a 1.  Calculate
#    the true positive rate and false positive rate.  This is a point
#    on the ROC curve.

require 'pp'

if ARGV[1] == nil
  abort("Usage: phase7-roc-auc.rb ground_truth.txt predict.txt")
else
  ground_truth_file = File.open(ARGV[0],"r")
  predict_file = File.open(ARGV[1],"r")
end

def trapezoidArea(x1, x2, y1, y2)
  return ((y1 + y2)/2.0) * (x2 - x1)
end

#
# Build a list of all the tags and clips in the ground truth file
#
clips = {}
tags = {}
begin
  predict_file.each_line do |line|
    a = line.chomp.split("\t")
    clip = a[0]
    tag = a[1]
    if (!clip.nil?)
      clips[clip] = 1
      tags[tag] = 1
    end
  end
rescue EOFError
  predict_file.close
end


#
# Read in the ground truth file into an hash of hashes
#
# ground_truth will end up looking something like:
#
# {"clip2"=> 
#    {"slow"=>1, "drums"=>0, "classical"=>0, "nice"=>0, "rock"=>1, "fast"=>0},
# "clip3"=>
#    {"slow"=>1, "drums"=>0, "classical"=>1, "nice"=>0, "rock"=>0, "fast"=>0},
# "clip4"=>
#    {"slow"=>0, "drums"=>0, "classical"=>0, "nice"=>1, "rock"=>0, "fast"=>0},
# "clip1"=>
#    {"slow"=>0, "drums"=>1, "classical"=>0, "nice"=>0, "rock"=>1, "fast"=>1}}
#
ground_truth = {}
begin
  ground_truth_file.each_line do |line|
    a = line.chomp.split("\t")
    clip = a[0]
    tag = a[1]
    if ground_truth[clip].nil?
      ground_truth[clip] = {}
      tags.each do |k,v|
        ground_truth[clip][k] = 0
      end
    end
    ground_truth[clip][tag] = 1
  end
rescue EOFError
  ground_truth_file.close
end


#
# Read in the predict file into an hash of hashes
#
predict_file = File.open(ARGV[1],"r")
predict = {}
begin
  predict_file.each_line do |line|
    a = line.chomp.split("\t")
    clip = a[0]
    tag = a[1]
    affinity = a[2]
    if predict[clip].nil?
      predict[clip] = {}
      tags.each do |k,v|
        predict[clip][k] = 0
      end
    end
    predict[clip][tag] = affinity
  end
rescue EOFError
  predict_file.close
end

#pp predict
#exit

#
# Generate the ROC curves
#
# Loop over the increments we want to search over and for each
# increment, loop over all the elements in the prediction file matrix.
# If the value in the matrix is less than the cutoff, put a 0 in the
# output matrix, if it is more, then put a 1.  Calculate the true
# positive rate and false positive rate.  This is a point on the ROC
# curve.
#


#################################################################################
# Generate the ROC curve for tags
#
#pp ground_truth
#pp predict
#pp tags

total_tag_auc = 0
calc_tag_auc = true

if (calc_tag_auc)

  # Loop over all the tags
  tags.each do |tag,z|

    #puts "------#{tag}-------"

    cutoff = 0.0
    increment = 0.01
    done = false

    points = []
    
    roc_calc_points = [0.0]
    predict.each do |k,v|
      roc_calc_points << v[tag].to_f
    end
    roc_calc_points.sort!
    #     predict.each do |k,v|
    #       puts "#{v[tag]}"
    #     end
    #pp roc_calc_points
    #     exit

    # Generate the points for the ROC curve
    #    while(!done) do
    roc_calc_points.each do |cutoff|

      #puts "cutoff=#{cutoff}"
      predict_cutoff = {}
      
      # Loop over all the clips
      clips.each do |clip,z|
        affinity = predict[clip][tag].to_f
        if (affinity.to_f >= cutoff)
          predict_cutoff[clip] = 1
        else
          predict_cutoff[clip] = 0
        end
      end

      #pp predict_cutoff

      # Calculate TP, FP, TN, FN
      true_positives = 0.0
      false_positives = 0.0
      true_negatives = 0.0
      false_negatives = 0.0

      predict_cutoff.each do |n|
        clip = n[0]
        present = n[1]
        true_positives  += 1  if ((present == 1) && (ground_truth[clip][tag] == 1))
        false_positives += 1  if ((present == 1) && (ground_truth[clip][tag] == 0))
        true_negatives  += 1  if ((present == 0) && (ground_truth[clip][tag] == 0))
        false_negatives += 1  if ((present == 0) && (ground_truth[clip][tag] == 1))
      end

      # puts "true_positives=#{true_positives}"
      # puts "false_positives=#{false_positives}"
      # puts "true_negatives=#{true_negatives}"
      # puts "false_negatives=#{false_negatives}"
      
      # Calculate the true positive rate and false positive rate
      # - If you then draw a graph of these, you will get the ROC curve
      tp_rate = true_positives / (true_positives + false_negatives)
      fp_rate = false_positives / (false_positives + true_negatives)
      #puts "#{fp_rate} #{tp_rate}"

      tp_rate = 0.0 if tp_rate.nan?
      fp_rate = 0.0 if fp_rate.nan?

      points << [fp_rate, tp_rate]

#       if (fp_rate == 0.0 || tp_rate == 0.0)
#         done = true 
#       end

      #cutoff += increment
    end
    auc = 0.0
    
    (1...points.size).each do |i|
      last = points[i]
      curr = points[i-1]
      
      trap = trapezoidArea(last[0], curr[0], last[1], curr[1]);
      
      #    puts "last[0]=#{last[0]} curr[0]=#{curr[0]} last[1]=#{last[1]} curr[1]=#{curr[1]} trap=#{trap}"
      auc += trap
    end
    
    puts "#{auc} #{tag}"
    $stdout.flush
    total_tag_auc += auc

  end

end

avg_tag_auc = total_tag_auc / tags.size

#################################################################################
# Generate the ROC curve for clips
#
#pp ground_truth
#pp predict
#pp clips

#exit

calc_clip_auc = true

if (calc_clip_auc)

  # Loop over all the clips
  total_clip_auc = 0
  clips.each do |clip,z|

    #puts "------#{clip}-------"
    cutoff = 0.0
    increment = 0.001
    done = false

    points = []

    roc_calc_points = [0.0]
    predict[clip].each do |k,v|
      roc_calc_points << v.to_f
    end
    roc_calc_points.sort!
    #pp predict[clip]
    #pp roc_calc_points
    #     exit

    # Generate the points for the ROC curve
    roc_calc_points.each do |cutoff|
      #    while (!done) do

      #puts "cutoff=#{cutoff}"

      predict_cutoff = {}

      #       puts "clip=#{clip}" 
      #       puts "predict[clip]"
      #       pp predict[clip]

      # Loop over all the tags
      predict[clip].each do |tag,affinity|

        #         puts "tag=#{tag} affinity=#{affinity}"

        if (affinity.to_f > cutoff)
          predict_cutoff[tag] = 1
        else
          predict_cutoff[tag] = 0
        end
      end

      #pp predict_cutoff

      # Calculate TP, FP, TN, FN
      true_positives = 0.0
      false_positives = 0.0
      true_negatives = 0.0
      false_negatives = 0.0

      predict_cutoff.each do |n|
        tag = n[0]
        present = n[1]
        true_positives  += 1  if ((present == 1) && (ground_truth[clip][tag] == 1))
        false_positives += 1  if ((present == 1) && (ground_truth[clip][tag] == 0))
        true_negatives  += 1  if ((present == 0) && (ground_truth[clip][tag] == 0))
        false_negatives += 1  if ((present == 0) && (ground_truth[clip][tag] == 1))
      end

      #         puts "true_positives=#{true_positives}"
#               puts "false_positives=#{false_positives}"
#               puts "true_negatives=#{true_negatives}"
#               puts "false_negatives=#{false_negatives}"
      
      # Calculate the true positive rate and false positive rate
      # - If you then draw a graph of these, you will get the ROC curve
      tp_rate = true_positives / (true_positives + false_negatives)
      fp_rate = false_positives / (false_positives + true_negatives)

      tp_rate = 0.0 if tp_rate.nan?
      fp_rate = 0.0 if fp_rate.nan?

      #puts "fp_rate=#{fp_rate}"
      #puts "tp_rate=#{tp_rate}"

      #puts "#{fp_rate} #{tp_rate}"

      points << [fp_rate, tp_rate]

      #       if (fp_rate == 0.0 || tp_rate == 0.0)
      #         done = true 
      #       end

      #cutoff += increment

    end

    #  pp points

    auc = 0.0

    (1...points.size).each do |i|
      last = points[i]
      curr = points[i-1]
      
      trap = trapezoidArea(last[0], curr[0], last[1], curr[1]);
      
      #puts "last[0]=#{last[0]} curr[0]=#{curr[0]} last[1]=#{last[1]} curr[1]=#{curr[1]} trap=#{trap}"
      auc += trap
    end
    
    puts "#{auc} #{clip}"
    $stdout.flush
    total_clip_auc += auc

  end

end

if (calc_clip_auc)
  avg_clip_auc = total_clip_auc / clips.size
end


if (calc_clip_auc)
  #puts "total_clip_auc=#{total_clip_auc}"
  puts "avg_clip_auc=#{avg_clip_auc}"
end

if (calc_tag_auc)
  #puts "total_tag_auc=#{total_tag_auc}"
  puts "avg_tag_auc=#{avg_tag_auc}"
end
