#!/usr/bin/ruby

require 'pp'

if ARGV[1] == nil
  abort("Usage: phase2-accuracy.rb ground_truth.txt predict.txt")
else
  ground_truth_file = File.open(ARGV[0],"r")
  predict_file = File.open(ARGV[1],"r")
end

#
# Build a list of all the tags and clips in the ground truth file
#
clips = {}
tags = {}
begin
  ground_truth_file.each_line do |line|
    a = line.chomp.split("\t")
    clip = a[0]
    tag = a[1]
    clips[clip] = 1
    tags[tag] = 1
  end
rescue EOFError
  ground_truth_file.close
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
ground_truth_file = File.open(ARGV[0],"r")
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

# puts "ground_truth"
# pp ground_truth

#
# Read in the predict file into an hash of hashes
#
predict = {}
begin
  predict_file.each_line do |line|
    a = line.chomp.split("\t")
    clip = a[0]
    tag = a[1]
    if predict[clip].nil?
      predict[clip] = {}
      tags.each do |k,v|
        predict[clip][k] = 0
      end
    end
    predict[clip][tag] = 1
  end
rescue EOFError
  predict_file.close
end

# puts "predict"
#pp predict
#exit

print_per_tag_stats = false
#print_per_tag_stats = true

total_precision = 0.0
total_recall = 0.0
total_accuracy = 0.0
total_f_score = 0.0

tags.each do |tag,z|

  if (print_per_tag_stats)	
    puts "----------#{tag}----------"
  end
  #
  # Calculate the average positive accuracy
  #
  true_positives = 0
  false_positives = 0
  true_negatives = 0
  false_negatives = 0

  clips.each do |clip,z|

    next if predict[clip].nil?

    present = predict[clip][tag]

    true_positives  += 1  if ((present == 1) && (ground_truth[clip][tag] == 1))
    false_positives += 1  if ((present == 1) && (ground_truth[clip][tag] == 0))
    true_negatives  += 1  if ((present == 0) && (ground_truth[clip][tag] == 0))
    false_negatives += 1  if ((present == 0) && (ground_truth[clip][tag] == 1))
  end

  if (print_per_tag_stats)
    puts "true_positives=#{true_positives}"
    puts "false_positives=#{false_positives}"
    puts "true_negatives=#{true_negatives}"
    puts "false_negatives=#{false_negatives}"
  end

  precision = true_positives.to_f / (true_positives + false_positives)
  recall = true_positives.to_f / (true_positives + false_negatives)
  accuracy = (true_positives.to_f + true_negatives) / (true_positives.to_f + true_negatives + false_positives + false_negatives)
  f_score = (2.0 * (precision * recall)) / (precision + recall)

  precision = 0.0 if precision.nan?
  recall = 0.0 if recall.nan?
  f_score = 0.0 if f_score.nan?

  if (print_per_tag_stats)
    puts "precision=#{precision}"
    puts "recall=#{recall}"
    puts "accuracy=#{accuracy}"
    puts "f_score=#{f_score}"
  end
  
  total_precision += precision
  total_recall += recall
  total_accuracy += accuracy
  total_f_score += f_score

end

avg_precision = total_precision / tags.size
avg_recall = total_recall / tags.size
avg_accuracy = total_accuracy / tags.size
avg_f_score = total_f_score / tags.size

puts "avg_precision=#{avg_precision}"
puts "avg_recall=#{avg_recall}"
puts "avg_accuracy=#{avg_accuracy}"
puts "avg_f_score=#{avg_f_score}"

puts "%5.3f %5.3f %5.3f %5.3f" % [avg_precision, avg_recall, avg_accuracy, avg_f_score]
