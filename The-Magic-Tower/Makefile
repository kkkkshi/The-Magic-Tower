CXX = clang++
CXXFLAGS = -std=c++17 -Wall -g -MMD
OBJECTS = main.o item.o player.o enemy.o floor.o game.o
DEPENDS = ${OBJECTS:.o=.d}
EXEC = cc3k

${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

.PHONY: clean
clean:
	rm -f ${OBJECTS} ${DEPENDS} ${EXEC}
