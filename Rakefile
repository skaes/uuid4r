require 'rubygems'
require 'rake/gempackagetask'

gemspec = eval(File.read(File.expand_path('../uuid4r.gemspec', __FILE__)))

Rake::GemPackageTask.new(gemspec) do |pkg|
  pkg.need_tar = true
end

desc "Clean build artefacts"
task :clean do
  system "rm -rf ext/Makefile ext/mkmf.log ext/*.o ext/*.bundle ext/*.so ext/*.bundle ext/conftest.dSYM"
end

desc "Build C extension"
task :build do
  system "cd ext && (test -f Makefile || ruby extconf.rb) && make"
end

desc "Run tests"
task :test => :build do
  system "cd ext && make check"
end

desc "Validate the gemspec"
task :validate do
  gemspec.validate
end
