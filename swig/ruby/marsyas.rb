# Load the shared object file (Will this work in Windows/OS X?)
require "marsyas_ruby.so"

# Creates the Marsyas Namespace
module Marsyas
	include Marsyas_ruby # Use the shared objects code

	# Create a class definition in this module
	def Marsyas.create_class name
		cls = Class.new MarSystem
		cls.send :class_variable_set,:@@name,name
		def cls.new name
			cname = class_variable_get :@@name
			o = @@msm.create(cname,name)
			# add control properties
			o.getControls.each do |name,_|
				gname = name.split("/").last
				sname = gname+"="
				o.instance_eval <<END;
				def #{gname}
					getControl "#{name}"
				end
				def #{sname} val
					updControl "#{name}",val
				end
END
			end
			return o
		end
		return cls
	end

	# Create a global MarSystemManager
	@@msm = MarSystemManager.new
	# And use it to generate the "automagic" classes
	@@msm.registeredPrototypes.each { |x| const_set x,(create_class x) }
end
