require 'rubygems'
Gem::manage_gems
require 'rake/gempackagetask'

spec = Gem::Specification.new do |s|  
  s.name    = "uuid4r"
  s.version = "0.1"
  s.author  = "Daigo Moriwaki"
  s.email   = "daigo@debian.org"
  s.rubyforge_project = "uuid4r"
  s.platform = Gem::Platform::RUBY
  s.summary = "This is a wrapper of OSSP uuid C library, and can generate and parse UUID."
  s.files = FileList['{ext,test}/**/*'].to_a
  s.require_path = '.'
  s.extensions << "ext/extconf.rb"
  s.test_files = ['test/test_uuid.rb']
end    

Rake::GemPackageTask.new(spec) do |pkg|
  pkg.need_tar = true
end



