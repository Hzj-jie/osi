
# this make file is generated by autocompile,
# with .autocompile configuration file,
# so change the configuration file or autocompile logic is prefered

all: \
 assert_test

assert_test: assert_test.cpp ../../../app_info/error_handle.hpp \
 ../../../app_info/error_type.hpp ../../../app_info/error_writer.hpp \
 ../../../app_info/k_assert.hpp ../../../app_info/../sync/lock.hpp \
 ../../../app_info/../sync/../utils/macro.hpp \
 ../../../app_info/../envs/exeinfo.hpp ../../../app_info/../envs/os.hpp \
 ../../../app_info/../envs/../template/singleton.hpp \
 ../../../app_info/../envs/deploys.hpp \
 ../../../app_info/../envs/../const/character.hpp \
 ../../../app_info/../envs/../utils/auto_removed_folder.hpp \
 ../../../app_info/../envs/../utils/sweeper.hpp \
 ../../../app_info/../envs/../utils/uuid.hpp \
 ../../../app_info/../envs/git.hpp ../../../app_info/../envs/nowadays.hpp \
 ../../../app_info/../envs/../utils/strutils.hpp \
 ../../../app_info/trace.hpp ../../../app_info/../utils/lazier.hpp \
 ../../../app_info/assert.hpp
	g++ assert_test.cpp -o assert_test -std=c++11 -Wall -Wno-unused-function -Wno-unused-variable -O3 -lboost_filesystem -lboost_system

clean:
	-rm assert_test

autocompile:
	autocompile > Makefile

