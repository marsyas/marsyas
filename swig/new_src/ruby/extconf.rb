require 'mkmf'
%w{vorbisfile mad}.each {|pkg| pkg_config pkg}
$CFLAGS += " " << "-I../../../src"
$LDFLAGS += " " << "-L../../../lib/release -lmarsyas -lstdc++"
create_makefile("marsyas")

open("Makefile", "a") do |mf|
	mf.puts <<EOF
Makefile : extconf.rb
	ruby $<
%_wrap.cxx : %.i
	swig -c++ -ruby $<
EOF
end
