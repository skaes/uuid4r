require 'mkmf'

if File.exist?(`which uuid-config`.chomp)
  $CFLAGS << " -Wall " << `uuid-config --cflags`.chomp
  $LDFLAGS << " " << `uuid-config --ldflags`.chomp
end

if !have_library('uuid')
  puts "OSSP uuid library required -- not found."
  exit 1
end
create_makefile('uuid4r')
File.open("Makefile", "a") << <<-EOT

check:	$(DLLIB)
	@$(RUBY) #{File.dirname(__FILE__)}/../test/test_uuid.rb
EOT
