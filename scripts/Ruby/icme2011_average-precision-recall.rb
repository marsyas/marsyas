#!/usr/bin/ruby

require 'pp'

#
# Calculate the precision and recall of the different signs given
# an input matrix file calculated by run-all-dtw-png.rb
#

if ARGV[0] == nil
  abort("Usage: average-precision-recall.rb in.txt")
else
  input_file = File.open(ARGV[0],"r")
end

# Read all the lines in the file and build up data structures for them
num_signs = 0
all_signs = []
all_lines = []
begin
  input_file.each_line do |line|
    a = line.split(" ")
    num_signs = a.length - 1
    a[0] =~ /([a-zA-Z0-9]*)/
    all_signs.push $1
    all_lines.push a[1..-1]
  end
rescue EOFError
  input_file.close
end

#
# Find just the unique signs for building our output table later
#
all_signs_uniq = all_signs.uniq

# A hash to store all the average precisions for each sign
all_signs_uniq_precision = {}
all_signs_uniq.each do |sign|
  all_signs_uniq_precision[sign] = []
end

# An array to store all the average precisions, for determining the
# global average precision
global_precision = []

#
# For each line in all_lines:
#
# 1) Build up an Array containing hashes with both the name of the
#    sign and the distance
#
# 2) Sort this array by distance
#
# 3) Calculate the average precision for each item in this list
#

# For each line in all_lines:
puts "Average Precision Recall for all calls"
average_precision_vector = []
top_one = []
all_lines.each_index do |index|

  # 1) Build up an Array containing hashes with both the name of the
  #    sign and the distance
  a = []
  all_lines[index].each_index do |i|
    a << { :sign => all_signs[i], :distance => all_lines[index][i].to_f }
  end

  # 2) Sort this array by distance
  b = a.sort_by { |n| n[:distance] }


  # 3) Calculate the average precision for each item in this list  
  total_precision = 0.0
  count = 0.0
  current_sign = all_signs[index]
  b.each_index do |i|
    if i == 0
      next
    end
    if (current_sign == b[i][:sign])
      count += 1
      total_precision += (count / (i.to_f))
      aa = "%.3f" % ((count / (i.to_f)))
      bb = "%.3f" % (total_precision)
      cc = "%.3f" % (total_precision/count)
      if (i == 1)
        top_one << 1
      end
                    
      #puts "match - current_sign=#{current_sign} b[i][:sign]=#{b[i][:sign]} count=#{count} i=#{i} v=#{aa} total_precision=#{bb} tp/count=#{cc}"

    else
      if (i == 1)
        top_one << 0
      end

      #puts "no ma - current_sign=#{current_sign} b[i][:sign]=#{b[i][:sign]} count=#{count} i=#{i} v=#{aa} total_precision=#{bb} tp/count=#{cc}"
    end
  end
  average_precision = total_precision / count

  puts "#{all_signs[index]} = #{average_precision}"

  all_signs_uniq_precision[all_signs[index]] << average_precision
  global_precision << average_precision

end

puts
puts "Average Precision Recall per call"
all_signs_uniq_precision.each do |k,value_array|
  total_precision = 0.0
  value_array.each do |v|
    total_precision += v
  end
  average_precision = total_precision / value_array.length
  puts "#{k} #{average_precision}"
end

puts
puts "Global Precision Recall"
total_precision = 0.0
global_precision.each do |v|
  total_precision += v
end
average_precision = total_precision / global_precision.length
puts "#{average_precision}"

puts "top_one"
# pp top_one

total_top_one = 0
top_one.each do |n|
  total_top_one += n
end
average_top_one = total_top_one.to_f / top_one.size().to_f
puts "Average top_one = #{average_top_one}"
