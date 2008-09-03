#!/usr/bin/ruby

#
# Use the program mirex_extract built with Marsyas to extract the
# features from a series of audio files.  The format of the
# featureExtractionListFile.txt file is one path per line in MIREX
# 2008 format.
#

#
# Pseudocode:
#
# 1) Change directory to the scratch folder
#
# 2) Count the number of lines in the extraction_list_file
#
# 3) Split up the extraction_list_file into num_cores chunks
#
# 4) Run num_cores threads of mirex_extract on each of the chunks
#
# 5) Wait for all mirex_extract jobs to finish
#
# 6) Join the output of each of the mirex_extract jobs into one big
# .arff file
# 


if ARGV[3] == nil
  abort("Usage: extractFeatures.rb /path/to/mirex_extract num_cores /path/to/scratch/folder /path/to/featureExtractionListFile.txt")
else
  path_to_mirex_extract = ARGV[0]
  num_cores = ARGV[1]
  scratch_folder = ARGV[2]
  extraction_list = ARGV[3]
end

puts "Extracting features from #{extraction_list} to #{scratch_folder}/features.arff using #{num_cores} cores"

#
# 1) Change directory to the scratch folder
#

Dir.chdir(scratch_folder)

#
# 2) Count the number of lines in the extraction_list_file
#
extraction_list_file = File.open(extraction_list,"r")
num_lines = 0
begin
  extraction_list_file.each_line do |line|
    num_lines += 1
  end
rescue 
  extraction_list_file.close
end

#puts "num_lines=#{num_lines}"


#
# 3) Split up the extraction_list_file into num_cores chunks 
#
extraction_list_file = File.open(extraction_list,"r")
i = 1
current_file = 1
output_file = File.open("input#{i}.mf","w")
file_open = true
begin
  extraction_list_file.each_line do |line|
    if (!file_open)
      output_file = File.open("input#{current_file}.mf","w")
      file_open = true
    end
    output_file.puts line
    i = i % ((num_lines.to_f / num_cores.to_f).to_i + 1)
    if (i == 0)
      output_file.close
      file_open = false
      current_file += 1
    end
    i += 1
  end
rescue EOFError
  extraction_list_file.close
end
output_file.close

#
# 4) Run num_cores threads of mirex_extract on each of the chunks
#

threads = []
(1..num_cores.to_i).each do |n|
  threads << Thread.new(n) do |thread|
    puts "Running ../mirex_extract input#{n}.mf output#{n}.arff"
    `#{path_to_mirex_extract} input#{n}.mf output#{n}.arff`
  end
end

#
# 5) Wait for all mirex_extract jobs to finish
#
threads.each { |thr| thr.join } 

#
# 6) Join the output of each of the mirex_extract jobs into one big
# .arff file
# 
`cat output1.arff > features.arff`
(2..num_cores.to_i).each do |n|
  `tail -n+71 output#{n}.arff >> features.arff`
end

