require 'mkmf'
$LDFLAGS = "-lossp-uuid"
$CFLAGS = "-Wall"
if !have_library('ossp-uuid')
  puts "OSSP uuid liberary required -- not found."
  exit 1
end
create_makefile('uuid4r')
File.open("Makefile", "a") << <<-EOT

check:	$(DLLIB)
	@$(RUBY) $(srcdir)/test/test_uuid.rb 
EOT
