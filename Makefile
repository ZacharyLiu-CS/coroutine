all : main
CC := clang++
FLAGS := -g -Wall
SRCS := ${wildcard *.cc}
main : main.cc ${SRCS}
	${CC} ${FLAGS} -o $@ $^

clean :
	rm main
