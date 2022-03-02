CC       = gcc -Wall -Werror -O3 -D_GLIBCXX_USE_NANOSLEEP
CC_DEBUG = gcc -Wall -Werror -D_GLIBCXX_DEBUG -g -fprofile-arcs -ftest-coverage
THREADS  = -pthread

EXECUTABLES = change purge

.PHONY: all
all: $(EXECUTABLES)

.PHONY: clean
clean:
	rm -f perf.data perf.data.old
	rm -f *.gcov *.gcda *.gcno *.gprof \#*# gmon.out
	rm -f a.out $(EXECUTABLES)

change: change.c match.c
	$(CC_DEBUG) $@.c -o $@
	./$@
	gprof $@ gmon.out > $@.gprof
	gcov $@ > /dev/null

purge: purge.c
	$(CC_DEBUG) $@.c -o $@
	./$@
	gprof $@ gmon.out > $@.gprof
	gcov $@ > /dev/null
