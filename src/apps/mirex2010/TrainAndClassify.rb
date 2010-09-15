#!/usr/bin/ruby

require 'pp'

#
# TrainAndClassify.rb
#
# Run kea on multiple processors and join the
# results together into an output file suitable for MIREX 2010.
#
# Pseudocode:
# 0) get current working dir and adjust the kea path
# 
# 1) Change directory to the scratch folder
#
# 2) Count the number of lines in the train_list file and get a list
#    of all the unique labels for putting into the .arff files we are
#    generating
#
# 3) Count the number of lines in the test_list file
#
# 4) Split train file into num_cores chunks
#
# 5) Split test file into num_cores chunks
#
# 6) Extract lines from features.arff for all the lines in the
#    train_list_file.  Add labels and output to train.arff
#
# 7) Extract lines from features.arff for the test_list_file and output to test.arff
#
# 8) Run mirex_train_and_predict on each core
#
# 9) Wait for all mirex_extract jobs to finish
#
# 10) For all the output files, iterate over all the lines, outputing
#     the pair of the filename and the label in the format of:
#     filename[TAB]label
#

if ARGV[4] == nil
  abort("Usage: TrainAndClassify.rb /path/to/kea /path/to/scratch/folder /path/to/trainListFile.txt /path/to/testListFile.txt /path/to/outputListFile.txt")
else
  path_to_mirex_train_and_predict = ARGV[0]
  scratch_folder = ARGV[1]
  train_list = ARGV[2]
  test_list = ARGV[3]
  output_list = ARGV[4]
end

working_dir = Dir.pwd

path_to_mirex_train_and_predict = working_dir + "/" + path_to_mirex_train_and_predict

num_cores = 1

puts "Running mirex_train_and_predict"
puts "path_to_mirex_train_and_predict = (#{path_to_mirex_train_and_predict})"
puts "num_cores = (#{num_cores.to_i})"
puts "scratch_folder = (#{scratch_folder})"
puts "input .arff file = (#{scratch_folder}/features.arff)"
puts "train_list = (#{train_list})"
puts "test_list = (#{test_list})"
puts "output_list = (#{output_list})"

#
# 1) Change directory to the scratch folder
#
Dir.chdir(scratch_folder)

#
# 2) Count the number of lines in the train_list file and get a list
#    of all the unique labels for putting into the .arff files we are
#    generating
#
train_list_file = File.open(train_list,"r")
train_num_lines = 0
train_labels = []
begin
  train_list_file.each_line do |line|
    s = line.split("\t")
    if (!s[1].nil?)
      train_labels.push(s[1].chomp)
    end
    train_num_lines += 1
  end
rescue EOFError
  train_list_file.close
end

train_labels = train_labels.uniq
# pp train_labels
# puts "train_num_lines=#{train_num_lines}"

#
# 3) Count the number of lines in the test_list_file
#
test_list_file = File.open(test_list,"r")
test_num_lines = 0
begin
  test_list_file.each_line do |line|
    test_num_lines += 1
  end
rescue EOFError 
  test_list_file.close
end

# puts "test_num_lines=#{test_num_lines}"


#
# 4) Split train file into num_cores chunks
#
train_list_file = File.open(train_list,"r")
i = 1
current_file = 1
output_file = File.open("train#{i}.mf","w")
file_open = true
begin
  train_list_file.each_line do |line|
    if (!file_open)
      output_file = File.open("train#{current_file}.mf","w")
      file_open = true
    end
    output_file.puts line
    i = i % ((train_num_lines.to_f / num_cores.to_f).to_i + 1)
    if (i == 0)
      output_file.close
      file_open = false
      current_file += 1
    end
    i += 1
  end
rescue EOFError
  train_list_file.close
end
output_file.close

#
# 5) Split test file into num_cores chunks
#
test_list_file = File.open(test_list,"r")
i = 1
current_file = 1
output_file = File.open("test#{i}.mf","w")
file_open = true
begin
  test_list_file.each_line do |line|
    if (!file_open)
      output_file = File.open("test#{current_file}.mf","w")
      file_open = true
    end
    output_file.puts line
    i = i % ((test_num_lines.to_f / num_cores.to_f).to_i + 1)
    if (i == 0)
      output_file.close
      file_open = false
      current_file += 1
    end
    i += 1
  end
rescue EOFError
  test_list_file.close
end
output_file.close

#
# 6) Read all the lines of features into a big array, with array index 0 being the
#    the feature vector and array index 1 being the label
#
features_header = ""
feature_vectors_array = []
feature_labels_array = []
features_file = File.open("#{scratch_folder}/features.arff")
in_data = false
features_file.each_line do |line|
  begin
    if (!in_data)
      if (line[0..16] == "@attribute output") 
        features_header += "@attribute output {" + train_labels.join(",") + "}\n"
      else
        features_header += line
      end
    end
    if (line.chomp == "@data")
      in_data = true
      next
    end
    if (in_data)
		# puts("#{line}")
		foo = line[0..2]
		if (foo == "% f")
		#	puts("Filename = #{line[11..-2]}")
      		   	feature_labels_array.push(line[11..-2])
		else
	        #   puts("Feature vector = #{line}")
      		   last_comma_pos = line.rindex(",")
      		   feature_vectors_array.push(line[0..last_comma_pos-1])
		end
    end
  rescue EOFError
    current_train_file.close
  end
end

# pp feature_vectors_array
#puts "feature_vectors_array.size()=#{feature_vectors_array.size()}"
# pp feature_labels_array

#
# 7) For each of the separate train{i}.mf files, extract lines from
#    features.arff for all the lines in the train_list_file.  Add
#    labels and output to train.arff
#
(1..num_cores.to_i).each do |n|
  puts "#7 (#{n})"
  current_train_input_file = File.open("train#{n}.mf","r")
  current_train_output_file = File.open("train#{n}.arff","w")
  # Output the header to the output .arff file
  current_train_output_file.puts features_header
  begin
    current_train_input_file.each_line do |line|
      # split the line up into the filename and label
      s = line.chomp.split("\t")
      filename = s[0]
      label = s[1]
      # find element in feature_labels_array with this label
      feature_index = feature_labels_array.index(filename)
      # Output the corresponding element in feature_vectors_array and the label
      current_train_output_file.puts "% filename #{filename}"
      current_train_output_file.puts "#{feature_vectors_array[feature_index]},#{label}"
    end
  rescue EOFError
  end
    current_train_input_file.close
    current_train_output_file.close
end

#
# 8) For each of the separate test{i}.mf files, extract lines from
#    features.arff for all the lines in the test_list_file.  Add
#    labels and output to test.arff
#
all_test_file_filenames = []
(1..num_cores.to_i).each do |n|
  current_test_input_file = File.open("test#{n}.mf","r")
  current_test_output_file = File.open("test#{n}.arff","w")
  # Output the header to the output .arff file
  current_test_output_file.puts features_header
  test_file_filenames = []
  begin
    current_test_input_file.each_line do |line|
      # split the line up into the filename and label
      s = line.chomp.split("\t")
      filename = s[0]
      test_file_filenames.push(filename)
      label = s[1]
      # find element in feature_labels_array with this label
      feature_index = feature_labels_array.index(filename)
      # Output the corresponding element in feature_vectors_array and the label
      if !feature_index.nil?
        current_test_output_file.puts "% filename #{filename}"
        current_test_output_file.puts "#{feature_vectors_array[feature_index]},#{train_labels[0]}"
      end
    end
    all_test_file_filenames.push(test_file_filenames)
  rescue EOFError
    current_test_input_file.close
    current_test_output_file.close
  end
end

#pp all_test_file_filenames

#
# 8) Run mirex_train_and_predict on each core
#
n = 1
puts "Running #{path_to_mirex_train_and_predict} train#{n}.arff test#{n}.arff"
system("#{path_to_mirex_train_and_predict} -m train_predict -w train#{n}.arff -tw test#{n}.arff -pr #{output_list}")

# #
# # 9) Wait for all mirex_extract jobs to finish
# #
# done = false
# while (!done)
#   a = `grep DONE out*txt | wc -l`.to_i
#   if (a == num_cores.to_i)
#     done = true
#   end
# end


#
# 10) For all the output files, iterate over all the lines, outputing
#     the pair of the filename and the label in the format of:
#     filename[TAB]label
#
#output_list_file = File.open(output_list,"w")
#(1..num_cores.to_i).each do |n|
#  out_file = File.open("out#{n}.txt","r")
#  begin
#    i = 0
#    out_file.each_line do |line|
#      if (line.chomp != "DONE")
#        # output_list_file.puts "#{all_test_file_filenames[n-1][i]}\t#{line}"
#        output_list_file.puts "#{line}"
#        i += 1
#      end
#    end
#  rescue EOFError
#    out_file.close
#  end
# end
# output_list_file.close
