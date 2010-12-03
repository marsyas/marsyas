#!/usr/bin/ruby

#
# Take a file with Audacity labels in it, and turn it into
# an .mpl file for input into Marsyas.
#
# *NB* : The Audacity label files for this task have only voice
# (labelled "voice") and orca (unlabelled) labels in them.  When we
# generate the .mpl file, turn all unlabelled regions into
# "background".
#
# More documentation about the .mpl file format can be found in:
# mDEV/test-ruby/convert-audacity-labeltxt-to-orcaannotator-marsyas.rb
#

if ARGV[0] == nil
  abort("Usage: generate-mpl.rb in.wav in.txt")
else
  input_wav = ARGV[0]
  input_file = File.open(ARGV[1],"r")
end

#
# Run sfinfo and process output to find the number of samples in the
# .wav file
#
sfinfo_output = `sfinfo #{input_wav}`

/Sampling Rate\s*([0-9.]*)/ =~ sfinfo_output
sampling_rate = $1.to_f
putf(sampling_rate)
/Duration\s*([0-9.]*)/ =~ sfinfo_output
duration = $1.to_f
puts(duration)
length_in_samples = (sampling_rate * duration).to_i

puts(length_in_samples)

#
# Go through the input file and figure out how many regions there are. 
#
regions = 0
# 
begin
  input_file.each_line do |line|
     if /([0-9.]*)\s*([0-9.]*)\s*([a-zA-Z]*)/ =~ line
       regions += 1
     else
       puts("the following line didn't match our regexp")
       abort(line)
     end
  end
rescue 
  input_file.close
end

# Then close the file and open it again so we can do the actual work
input_file.close
input_file = File.open(ARGV[1],"r")

# Output the header
puts regions
puts 1
puts length_in_samples

# chords_array = ["A", "A/2", "A/3", "A/4", "A/5", "A/6", "A/7", "A/9", "A/b3", "A/b6", "A/b7", "A:(1)", "A:(1,2,4)", "A:7", "A:7(#9)", "A:7(*5,13)", "A:7(13)", "A:7/3", "A:7/5", "A:7/b7", "A:9", "A:9(11)", "A:aug", "A:aug/#5", "A:dim/b3", "A:dim/b5", "A:dim7", "A:dim7/b3", "A:maj(*5)", "A:maj(9)/3", "A:maj/9", "A:maj6", "A:maj7", "A:maj7/5", "A:min", "A:min(*5)", "A:min(2)", "A:min/4", "A:min/5", "A:min/6", "A:min/b3", "A:min/b7", "A:min6", "A:min7", "A:min7(*5,b6)", "A:min7(*b3)", "A:min9", "A:sus2", "A:sus4", "A:sus4(2)", "A:sus4/5", "Ab", "Ab/5", "Ab/7", "Ab/b7", "Ab:7", "Ab:aug", "Ab:maj(2)/2", "Ab:maj(9)", "Ab:maj6", "Ab:maj7", "Ab:maj9", "Ab:min", "Ab:min7", "B", "B/3", "B/5", "B/6", "B/7", "B:(1)", "B:(6)", "B:(b3,5)", "B:7", "B:7(#9)", "B:9", "B:aug", "B:aug/3", "B:dim", "B:dim7", "B:dim7/b9", "B:hdim7/b3", "B:hdim7/b7", "B:maj(*3)", "B:maj/9", "B:maj6", "B:maj7", "B:min", "B:min/5", "B:min/6", "B:min/7", "B:min/b3", "B:min/b7", "B:min7", "B:sus2", "B:sus4", "Bb", "Bb/3", "Bb/5", "Bb/7", "Bb/b7", "Bb:(1)", "Bb:7", "Bb:dim7/5", "Bb:dim7/7", "Bb:maj", "Bb:maj(9)/9", "Bb:maj/9", "Bb:maj6", "Bb:maj7", "Bb:maj7/5", "Bb:min", "Bb:min/5", "Bb:min/b3", "Bb:min7", "Bb:sus2(b7)", "Bb:sus4(9)", "C", "C#", "C#/3", "C#:(1,b3)", "C#:(1,b3)/b3", "C#:7", "C#:dim", "C#:dim/b3", "C#:hdim7", "C#:maj7(*b5)", "C#:maj7/3", "C#:min", "C#:min/4", "C#:min/5", "C#:min/b7", "C#:min7", "C#:sus4", "C/2", "C/3", "C/4", "C/5", "C/6", "C/7", "C/9", "C:(1)", "C:(1,5)", "C:7", "C:7/3", "C:7/5", "C:9", "C:9(*3)", "C:aug", "C:dim7", "C:dim7/2", "C:maj(#11)", "C:maj(#4)/5", "C:maj(2)", "C:maj(4)", "C:maj(9)", "C:maj6", "C:maj6/5", "C:maj7", "C:maj7(*5)", "C:maj7/7", "C:maj9", "C:min", "C:min(*b3)", "C:min6", "C:sus4", "C:sus4/4", "D", "D#", "D#:(1,4)", "D#:7", "D#:dim", "D#:dim/b5", "D#:dim7", "D#:hdim7", "D#:min", "D/2", "D/3", "D/5", "D/6", "D/7", "D/b7", "D:(1)", "D:(1,4)", "D:(1,5)", "D:7", "D:7(#9)", "D:7/2", "D:7/5", "D:9", "D:9/5", "D:dim", "D:dim7", "D:maj(*1)/#1", "D:maj(*3)", "D:maj(11)", "D:maj(2)", "D:maj(9)", "D:maj/2", "D:maj6", "D:maj6/5", "D:maj7", "D:min", "D:min/2", "D:min/4", "D:min/5", "D:min/6", "D:min/b3", "D:min/b7", "D:min7", "D:min7(*b3)", "D:min7(2,*b3,4)", "D:min7(4)/5", "D:min7(4)/b7", "D:min7/4", "D:min7/b3", "D:min7/b7", "D:min9", "D:sus4", "D:sus4(2)", "D:sus4(9)", "D:sus4(b7)", "D:sus4/5", "Db", "Db/5", "Db:7", "Db:maj7", "Db:min", "E", "E/#4", "E/2", "E/3", "E/4", "E/5", "E/6", "E/7", "E/b6", "E/b7", "E:(1)", "E:(1,2,5,b6)", "E:(1,4)", "E:(1,5)", "E:(1,b7)/b7", "E:7", "E:7(#9)", "E:7/3", "E:7/5", "E:9", "E:aug", "E:aug(9,11)", "E:dim/b3", "E:dim7/b3", "E:maj(b9)", "E:maj6", "E:maj6/b7", "E:maj7", "E:min", "E:min(*3)/5", "E:min(*5)", "E:min(*5)/b7", "E:min(*b3)/5", "E:min(2)", "E:min(9)", "E:min/3", "E:min/5", "E:min/6", "E:min/b3", "E:min/b7", "E:min6", "E:min7", "E:min7(*5)/b7", "E:min7(4)", "E:min7/b3", "E:min7/b7", "E:sus2(b7)", "E:sus4", "E:sus4(2)", "E:sus4(b7)", "Eb", "Eb/2", "Eb/3", "Eb/5", "Eb/6", "Eb:7", "Eb:7/b7", "Eb:9", "Eb:aug", "Eb:dim", "Eb:maj6", "Eb:min", "Eb:min7", "Eb:sus4", "F", "F#", "F#/5", "F#:(1,4,b5)", "F#:(1,4,b7)", "F#:7", "F#:7(#9)", "F#:9", "F#:aug", "F#:dim", "F#:dim/b3", "F#:dim/b7", "F#:hdim7", "F#:hdim7/b7", "F#:maj(9)", "F#:min", "F#:min/5", "F#:min/b3", "F#:min6", "F#:min7", "F#:min9", "F#:minmaj7", "F#:sus4", "F/3", "F/5", "F/6", "F/7", "F/9", "F/b7", "F:(1)", "F:7", "F:7(b9)", "F:7/b7", "F:9", "F:aug", "F:dim", "F:maj(*3)", "F:maj(9)", "F:maj(9)/5", "F:maj(9)/6", "F:maj(9)/9", "F:maj/9", "F:maj6", "F:maj6/5", "F:maj7", "F:maj9", "F:maj9(*7)", "F:min", "F:min/5", "F:min/b7", "F:min6", "F:min6/5", "F:min6/b3", "F:min7", "F:min7/b3", "G", "G#", "G#:(1)", "G#:7", "G#:aug", "G#:dim7", "G#:hdim7", "G#:min", "G#:min7", "G#:min7/b3", "G/#4", "G/2", "G/3", "G/5", "G/b7", "G:(1)", "G:(1,5)", "G:(1,b3,4)/b3", "G:(6)", "G:(7)", "G:7", "G:7/2", "G:7/3", "G:7/b2", "G:7/b3", "G:9", "G:9(*3)", "G:9(*3,11)", "G:aug", "G:dim", "G:dim/b3", "G:dim7", "G:maj(*1)/5", "G:maj/9", "G:maj6", "G:maj6(9)", "G:maj6/2", "G:maj6/3", "G:maj6/5", "G:maj7", "G:maj7/3", "G:maj7/5", "G:min", "G:min(4)", "G:min(9)/b3", "G:min/4", "G:min/5", "G:min/b3", "G:min/b7", "G:min7", "G:minmaj7", "G:minmaj7/5", "G:minmaj7/b3", "G:sus4", "G:sus4(2)", "G:sus4(2)/2", "G:sus4(b7)", "G:sus4/5", "Gb", "Gb:min", "N"]

chords_array = ["whale"]
my_chords = []

begin
  input_file.each_line do |line|
     if /([0-9.]*)\s*([0-9.]*)\s*([a-zA-Z]*)/ =~ line
       _start = ($1.to_f * 44100).round.to_i
       _end = ($2.to_f * 44100).round.to_i

       _label = $3
       _classid = chords_array.index(_label)

       if (_classid.nil?)
         puts "label didn't match any of the chords in chords_array"
         abort line
       end


       # Write out data for each section
       puts _start.to_s
       puts _classid
       puts _end
       puts _label
     
     else
       puts("the following line didn't match our regexp")
       abort(line)
     end
  end
rescue Exception => e
#  puts e.message
  input_file.close
end





