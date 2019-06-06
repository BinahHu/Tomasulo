objects=Var.o StationItem.o RegItem.o Instruction.o BoardItem.o Tomasulo.o util.o

all: tomasulo

tomasulo: $(objects) def.h util.h 
	g++ -std=c++11 $(objects) -o tomasulo
	
clean:
	rm *.o
