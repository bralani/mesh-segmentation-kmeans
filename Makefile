# Nome dell'eseguibile
TARGET = executable

# Compilatore e flag
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -O2 -Iinclude -Iinclude/matplotlibcpp -Iinclude/csv-parser

# Specifica manuale dei file sorgenti e dei file oggetto
SRC = tests/main.cpp src/plot.cpp
OBJ = $(SRC:%.cpp=build/%.o) # Convert .cpp to .o in the build/ directory

# Regole principali
all: $(TARGET)

# Regola per creare l'eseguibile
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

# Regola per compilare i singoli file sorgenti
build/%.o: %.cpp
	@mkdir -p $(@D) # Ensure the build directory exists
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regola per pulire i file generati
clean:
	rm -rf build $(TARGET)
