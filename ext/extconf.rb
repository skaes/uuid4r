require 'mkmf'

lib="uuid"

if File.exist?(`which uuid-config`.chomp)
  lib = `uuid-config --libs`.chomp.gsub(/-l/, '')
  $CFLAGS << " -Wall " << `uuid-config --cflags`.chomp
  $LDFLAGS << " " << `uuid-config --ldflags`.chomp
end

unless have_library(lib, 'uuid_export')
  puts "OSSP uuid library '#{lib}' required -- not found."
  exit 1
end

create_makefile('uuid4r')

File.open("Makefile", "a") << <<-EOT

check:	$(DLLIB)
	@$(RUBY) #{File.dirname(__FILE__)}/../test/test_uuid.rb
EOT
