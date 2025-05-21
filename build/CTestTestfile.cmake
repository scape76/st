# CMake generated Testfile for 
# Source directory: /home/dan/code/st
# Build directory: /home/dan/code/st/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(UnitTests "/home/dan/code/st/build/unit_tests")
set_tests_properties(UnitTests PROPERTIES  _BACKTRACE_TRIPLES "/home/dan/code/st/CMakeLists.txt;74;add_test;/home/dan/code/st/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
