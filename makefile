#
# Copyright Erik Bryant (erikbryantology@gmail.com)
# Gnu All-Permissive http://www.gnu.org/philosophy/license-list.html#GNUAllPermissive
#

CC       = g++ -Wall -Werror -Weffc++ -O3 -D_GLIBCXX_USE_NANOSLEEP
CC_DEBUG = g++ -Wall -Werror -Weffc++ -D_GLIBCXX_DEBUG -g -fprofile-arcs -ftest-coverage -pg
C11      = -std=c++11
THREADS  = -pthread
CPPCHECK = ../../../cppcheck-1.58/cppcheck

EXECUTABLES = change

.PHONY: all
all: $(EXECUTABLES)

.PHONY: clean
clean:
	rm -f perf.data perf.data.old
	rm -f *.gcov *.gcda *.gcno *.gprof \#*# gmon.out
	rm -f a.out $(EXECUTABLES)

change: change.c match.c purge.c
	$(CPPCHECK) $^
	$(CC_DEBUG) $@.c -o $@
	./$@
	gprof $@ gmon.out > $@.gprof
	gcov $@ > /dev/null
