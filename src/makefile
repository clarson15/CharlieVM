PARAMS = -std=c++11 -I ../include -Wpedantic -Wall
OBJS1 = main.cpp VM.cpp
OBJS2 = asm.cpp
main:
	g++ ${PARAMS} ${OBJS2} -o ../a.sm
	g++ ${PARAMS} ${OBJS1} -o ../a.out
run:
	../a.sm ../test.asm ../test.rom
	../a.out ../test.rom
