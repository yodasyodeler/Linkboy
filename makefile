OBJS= common.o baseMBC.o  MBC1.o MBC3.o mmu.o CycleLogger.o FileLogger.o SmartLogger.o main.o gbz80.o ppu.o timer.o Directory.o Rectangle.o Label.o Button.o TextBox.o  MainMenu.o FileMenu.o NetworkMenu.o LobbyMenu.o SFML_Display.o linkboyClient.o linkboy.o  
#MBC2.o MBC5.o
CC= g++

VER= -std=gnu++11

CFLAGS=-Wall -c $(VER)
LFLAGS=-Wall 

LIBS=-lsfml-graphics -lsfml-window -lsfml-system -lsfml-network

INCDIR=-Iinclude 

SRC= source/
INC= include/

EXECUTABLE= release/linkboy

all: linkboy

linkboy: $(OBJS)
	$(CC) $(VER) $(OBJS) $(INCDIR) -o $(EXECUTABLE) $(LIBS)

main.o: linkboy-SFML/main.cpp
	$(CC) $(CFLAGS) linkboy-SFML/main.cpp $(INCDIR) 

linkboy.o: $(SRC)linkboy.cpp $(INC)linkboy.h
	$(CC) $(CFLAGS) $(SRC)linkboy.cpp $(INCDIR) 

gbz80.o: $(SRC)gbz80.cpp $(INC)gbz80.h
	$(CC) $(CFLAGS) $(SRC)gbz80.cpp $(INCDIR)

mmu.o: $(SRC)mmu.cpp $(INC)mmu.h
	$(CC) $(CFLAGS) $(SRC)mmu.cpp $(INCDIR) 

ppu.o: $(SRC)ppu.cpp $(INC)ppu.h
	$(CC) $(CFLAGS) $(SRC)ppu.cpp $(INCDIR) 

timer.o: $(SRC)timer.cpp $(INC)timer.h
	$(CC) $(CFLAGS) $(SRC)timer.cpp $(INCDIR) 

common.o: $(SRC)common.cpp $(INC)common.h
	$(CC) $(CFLAGS) $(SRC)common.cpp $(INCDIR) 

Directory.o: $(SRC)Directory.cpp $(INC)Directory.h
	$(CC) $(CFLAGS) $(SRC)Directory.cpp $(INCDIR) 

baseMBC.o: $(SRC)MBC/baseMBC.cpp $(INC)MBC/baseMBC.h
	$(CC) $(CFLAGS) $(SRC)MBC/baseMBC.cpp $(INCDIR)

MBC1.o: baseMBC.o $(SRC)MBC/MBC1.cpp $(INC)MBC/MBC1.h
	$(CC) $(CFLAGS) $(SRC)MBC/MBC1.cpp $(INCDIR) 

# MBC2.o: baseMBC.o $(SRC)MBC/MBC2.cpp $(INC)MBC/MBC2.h
# 	$(CC) $(CFLAGS) $(SRC)MBC/MBC2.cpp $(INCDIR) 

MBC3.o: baseMBC.o $(SRC)MBC/MBC3.cpp $(INC)MBC/MBC3.h
	$(CC) $(CFLAGS) $(SRC)MBC/MBC3.cpp $(INCDIR) 
	
# MBC5.o: baseMBC.o $(SRC)MBC/MBC5.cpp $(INC)MBC/MBC5.h
# 	$(CC) $(CFLAGS) $(SRC)MBC/MBC5.cpp $(INCDIR) 

Rectangle.o: $(SRC)SFML-GUI/Rectangle.cpp $(INC)SFML-GUI/Rectangle.h
	$(CC) $(CFLAGS) $(SRC)SFML-GUI/Rectangle.cpp $(INCDIR) 

Label.o: Rectangle.o $(SRC)SFML-GUI/Label.cpp $(INC)SFML-GUI/Label.h
	$(CC) $(CFLAGS) $(SRC)SFML-GUI/Label.cpp $(INCDIR) 

Button.o: Label.o $(SRC)SFML-GUI/Button.cpp $(INC)SFML-GUI/Button.h
	$(CC) $(CFLAGS) $(SRC)SFML-GUI/Button.cpp $(INCDIR) 

TextBox.o: Button.o $(SRC)SFML-GUI/TextBox.cpp $(INC)SFML-GUI/TextBox.h
	$(CC) $(CFLAGS) $(SRC)SFML-GUI/TextBox.cpp $(INCDIR) 


#SFML_Display.o CycleLogger.o FileLogger.o SmartLogger.o

FileMenu.o: $(SRC)SFML-GUI/FileMenu.cpp $(INC)SFML-GUI/FileMenu.h
	$(CC) $(CFLAGS) $(SRC)SFML-GUI/FileMenu.cpp $(INCDIR) 

MainMenu.o: $(SRC)SFML-GUI/MainMenu.cpp $(INC)SFML-GUI/MainMenu.h
	$(CC) $(CFLAGS) $(SRC)SFML-GUI/MainMenu.cpp $(INCDIR) 

NetworkMenu.o: $(SRC)SFML-GUI/NetworkMenu.cpp $(INC)SFML-GUI/NetworkMenu.h
	$(CC) $(CFLAGS) $(SRC)SFML-GUI/NetworkMenu.cpp $(INCDIR) 

LobbyMenu.o: $(SRC)SFML-GUI/LobbyMenu.cpp $(INC)SFML-GUI/LobbyMenu.h
	$(CC) $(CFLAGS) $(SRC)SFML-GUI/LobbyMenu.cpp $(INCDIR) 

SFML_Display.o: $(SRC)SFML-GUI/SFML_Display.cpp
	$(CC) $(CFLAGS) $(SRC)SFML-GUI/SFML_Display.cpp $(INCDIR) 

CycleLogger.o: $(SRC)DebugLogger/CycleLogger.cpp $(INC)DebugLogger/CycleLogger.h $(INC)DebugLogger/BaseLogger.h
	$(CC) $(CFLAGS) $(SRC)DebugLogger/CycleLogger.cpp $(INCDIR) 

FileLogger.o: $(SRC)DebugLogger/FileLogger.cpp $(INC)DebugLogger/CycleLogger.h $(INC)DebugLogger/BaseLogger.h
	$(CC) $(CFLAGS) $(SRC)DebugLogger/FileLogger.cpp $(INCDIR) 

SmartLogger.o: $(SRC)DebugLogger/SmartLogger.cpp $(INC)DebugLogger/SmartLogger.h $(INC)DebugLogger/BaseLogger.h
	$(CC) $(CFLAGS) $(SRC)DebugLogger/SmartLogger.cpp $(INCDIR) 

linkboyClient.o: $(SRC)linkboyClient.cpp $(INC)linkboyClient.h 
	$(CC) $(CFLAGS) $(SRC)linkboyClient.cpp $(INCDIR) 

clean:
	rm $(EXECUTABLE) *.o
