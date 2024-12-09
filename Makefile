# Nome dell'eseguibile
TARGET = executable

# Compilatore e flag
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -O2 -Iinclude

# Specifica manuale dei file sorgenti e dei file oggetto
SRC = tests/main.cpp
OBJ = build/main.o

# Regole principali
all: $(TARGET)

# Regola per creare l'eseguibile
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

# Regole per compilare i singoli file sorgenti

build/main.o: tests/main.cpp 
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/KDTree.o: src/KDTree.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regola per pulire i file generati
clean:
	rm -rf build $(TARGET)
