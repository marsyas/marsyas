require "marsyas"

# Creates a Network of MarSystems to read, process & play a sound file
# Uses the Ruby specific 'automagical' features

include Marsyas # Avoid typing Marsyas:: all the time

# Create the MarSystems using the automatic Classes
pipe = Series.new "pipe"
file = SoundFileSource.new "file"
gain = Gain.new "gain"
down = DownSampler.new "down"
sink = AudioSink.new "sink"

# Attach all the MarSystems together
[file,gain,down,sink].each {|x| pipe.addMarSystem x }

def usage
	puts <<END ; exit
usage: #{$0} [options] filename
	--gain,-g	<real>	Set Gain (Default 1.0)
	--downsample,-d	<int>	Set Downsampling factor (Default 1)
	--usage,-u		This message
	--help,-h		Also this message
END
end

# Default values for parameters
arg_down = 1
arg_gain = 1.0
arg_file = nil

if ARGV.length == 0 then usage end

# Parse Command line
while ARGV.length > 0
	arg = ARGV.shift
	case arg
	when "-d","--downsample" 
		arg_down = ARGV.shift.to_i
	when "-g","--gain" 
		arg_gain = ARGV.shift.to_f
	when "-u","-h","--help","--usage" 
		usage
	else 
		arg_file = arg
	end
end

if arg_file.nil? then fail "Have to provide a file name!" end
if not File.exists? arg_file then fail "#{arg_file} must exist!" end
if not File.file? arg_file then fail "#{arg_file} must be a file!" end

# Set control values using automatic properties
file.filename = arg_file
down.factor = arg_down
gain.gain = arg_gain

# Update all MarSystems to handle changes
pipe.update

# Loop untill file is finished
while file.notEmpty
	pipe.tick()
end
