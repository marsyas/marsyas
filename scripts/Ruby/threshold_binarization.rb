#!/usr/bin/ruby

#
# This program that takes a ground truth file and an
# affinity matrix and generate a tags file from this where only the tags
# above a certain threshold of affinity are included.
#
# The threshold is determined by looking at the total number of tags of
# each type in the ground truth file.  If there are more tags, then the
# threshold is set higher.
#
# The final result of this is a tags file with approximately the same
# number of tags of each type as in the ground truth file.
#
# For example, if there were a lot of "rock" tags in the ground truth
# file, the output file would also have a lot of rock tags.  If there
# were very few "spacey" tags, the output would also have very few
# "spacey" tags.
#
# Pseudocode: 
#
# 1) Count how many of each tag there are in the ground truth file
#
# 2) Read affinities file
#
# 3) Count how many of each tag in the affinities file (NOTE: this equals the nr. of songs in test file as we have one affinity per tag for each song in the test file)
#
# 4) Sort each tag by affinity
#
# 5) Estimate the proportion of each tag in the test file based on the proportion of each tag in the ground truth file
#
# 6) For each tag, get the threshold affinity as given by the tags sorted by affinity in proportion to the frequency of each tag in the ground truth file
#

require 'pp'

if ARGV[1] == nil
  abort("Usage: threshold_binarization.rb ground_truth.txt affinities.txt")
else
  ground_truth_file = File.open(ARGV[0],"r")
  affinities_file = File.open(ARGV[1],"r")
end

#
# Count nr of lines and how many of each tag in the ground truth file
#
tags = {}
songsGT = {}
begin
  ground_truth_file.each_line do |line|
    a = line.chomp.split("\t")
    tag = a[1]
    song = a[0]
    if (tags[tag].nil?)
      tags[tag] = 0
    end
    tags[tag] += 1
    
    if (songsGT[song].nil?)
      songsGT[song] = 0
    end
    songsGT[song] += 1
  end
rescue EOFError
  ground_truth_file.close
end

#
# Read in the affinities file into an hash of hashes
#
affinities_file = File.open(ARGV[1],"r")
affinities = {}
songsAff = {}
begin
  affinities_file.each_line do |line|
    a = line.chomp.split("\t")
    clip = a[0]
    tag = a[1]
    affinity = a[2]
    if affinities[clip].nil?
      affinities[clip] = {}
      tags.each do |k,v|
        affinities[clip][k] = 0
      end
    end
    affinities[clip][tag] = affinity
    
    if (songsAff[clip].nil?)
      songsAff[clip] = 0
    end
    songsAff[clip] += 1
  end
rescue EOFError
  affinities_file.close
end

#pp affinities

#
# Sort each tag by affinity
#
sorted_tag_affinities = {}
tags.each do |tag,z|
  a = []
  affinities.each do |song,aff|
    a << aff[tag]
  end
  sorted_tag_affinities[tag] = a.sort.reverse
end

#
# Estimate the proportion of each tag in the test file based on the proportion of each tag in the ground truth file
#
# Count the nr. of occurrences of each tag in ground truth and affinities files
#
totalGt_length = 0
tagGt_length = {}
tagAff_length = {}
tags.each do |tag,z|
	tagGt_length[tag] = tags[tag] # nr of occurrences of each tag in the ground truth file
	tagAff_length[tag] = sorted_tag_affinities[tag].length # nr of occurrences of each tag in the affinities file
	totalGt_length += tagGt_length[tag]
end

#
# Calc. nr. of songs in ground truth and affinities files and the proportion between both
#
nrSongsGt = songsGT.length
nrSongsAff = songsAff.length
songsGt2Aff = Float(nrSongsAff) / Float(nrSongsGt)

#
# Calculate thresholds based on the ground truth file and the former proportion
#
tag_cutoff_affinities = {}
tags.each do |tag,z|
  tag_cutoff = (Float(songsGt2Aff) * Float(tagGt_length[tag])).round
  tag_cutoff_affinities[tag] = sorted_tag_affinities[tag][tag_cutoff]
end


#pp tag_cutoff_affinities

#
# Loop over each song and only output the songs that
# have an affinity higher than the cutoff
#
affinities.each do |song,tags|
  tags.each do |tag,affinity|
    if (affinity > tag_cutoff_affinities[tag])
       puts "#{song}\t#{tag}"
    end
  end
end
