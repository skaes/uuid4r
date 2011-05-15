Gem::Specification.new do |s|
  s.name    = "uuid4r"
  s.version = "0.1.2"
  s.authors = ["Daigo Moriwaki", "Stefan Kaes"]
  s.email   = ["daigo@debian.org", "skaes@railsexpress.de"]
  s.rubyforge_project = "uuid4r"
  s.platform = Gem::Platform::RUBY
  s.summary = "This generates and parses UUID, based on OSSP uuid C library."
  s.description = <<-EOF
    This library generates and parses Universally Unique Identifier (UUID),
    based on OSSP uuid C library. So, libossp-uuid library is pre-required.
    OSSP uuid (http://www.ossp.org/pkg/lib/uuid/) is a ISO-C:1999 application
    programming interface (API) for the generation of DCE 1.1, ISO/IEC
    11578:1996 and RFC 4122 compliant UUID. It supports DCE 1.1 variant UUIDs
    of version 1 (time and node based), version 3 (name based, MD5), version 4
    (random number based) and version 5 (name based, SHA-1).
  EOF
  s.homepage = "http://uuid4r.rubyforge.org"

  s.files = FileList['ext/extconf.rb', 'ext/*.c', 'test/**/*'].to_a
  s.require_path = '.'
  s.extensions << "ext/extconf.rb"
  s.test_files = ['test/test_uuid.rb']

  s.has_rdoc = true
  s.extra_rdoc_files = ['README']
  s.rdoc_options << '--title' << 'UUID4R' <<
                    '--main' << 'README' <<
                    '--line-numbers'
end

