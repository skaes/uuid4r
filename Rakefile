require 'bundler/gem_tasks'

gemspec = eval(File.read(File.expand_path('../uuid4r.gemspec', __FILE__)))

desc "Clean build artefacts"
task :clean do
  system "rm -rf ext/Makefile ext/mkmf.log ext/*.o ext/*.bundle ext/*.so ext/*.bundle ext/conftest.dSYM"
end

desc "Build C extension"
task :compile do
  system "cd ext && (test -f Makefile || ruby extconf.rb) && make"
end

desc "Run tests"
task :test => :compile do
  system "cd ext && make check"
end

desc "Validate the gemspec"
task :validate do
  gemspec.validate
end

task :default => :test
