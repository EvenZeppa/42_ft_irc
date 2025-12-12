# 🛠 Compilateur et flags
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
LDFLAGS = -L /usr/local -lbnf

# 🏆 Nom du projet
PROJECT_NAME = ircserv

# 🖥️ Détection de l'OS
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
	CXXFLAGS   += -I/usr/include
	LDFLAGS    += -L/usr/lib
else ifeq ($(UNAME_S), Darwin)
	CXXFLAGS   += -I/opt/homebrew/include
	LDFLAGS    += -L/opt/homebrew/lib -framework Cocoa
else ifeq ($(UNAME_S), Windows_NT)
	CXXFLAGS   +=
	LDFLAGS    +=
endif

# 📂 Répertoires
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include
TEST_DIR = test
TEST_OBJ_DIR = $(OBJ_DIR)/test

# 📌 Fichiers sources et objets (sans main.cpp pour les tests, excluant le dossier test/)
SRC_FILES = $(filter-out ./test/%, $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp))
MAIN_OBJ = $(OBJ_DIR)/main.o
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
OBJ_WITHOUT_MAIN = $(filter-out $(MAIN_OBJ), $(OBJ_FILES))

# 📌 Fichiers de test (excluant main_test et integration_test)
TEST_FILES = $(filter-out $(TEST_DIR)/main_test.cpp $(TEST_DIR)/integration_test.cpp, $(wildcard $(TEST_DIR)/*.cpp) $(wildcard $(TEST_DIR)/*/*.cpp))
TEST_OBJ_FILES = $(patsubst $(TEST_DIR)/%.cpp, $(TEST_OBJ_DIR)/%.o, $(TEST_FILES))

# 📌 Fichiers runner de test
TEST_RUNNER_SRC = $(TEST_DIR)/main_test.cpp $(TEST_DIR)/integration_test.cpp
TEST_RUNNER_OBJ = $(patsubst $(TEST_DIR)/%.cpp, $(TEST_OBJ_DIR)/%.o, $(TEST_RUNNER_SRC))

# 📚 Inclusion des headers
INCLUDES = -I$(INCLUDE_DIR)

# 🎯 Règle principale
all: $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJ_FILES) $(LDFLAGS) -o $@
	@echo "✅ Compilation terminée !"

# 🛠️ Compilation des fichiers objets (prise en charge des sous-dossiers)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@
	@echo "🔨 Compilé : $< -> $@"

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# 🧹 Nettoyage des fichiers objets
clean:
	rm -rf $(OBJ_DIR)
	@echo "🗑️  Fichiers objets nettoyés."

# 🗑️ Nettoyage complet (objets et exécutable)
fclean: clean
	rm -f $(PROJECT_NAME) test_runner
	@echo "🗑️  Nettoyage complet effectué."

# 🔄 Reconstruction
re: fclean all

# ▶️ Exécution du projet avec support d'arguments (optionnel)
run: all
	./$(PROJECT_NAME) $(ARGS)

# 🧪 Compilation des fichiers objets de tests
$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp | $(TEST_OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@
	@echo "🧪 Compilé test: $< -> $@"

$(TEST_OBJ_DIR):
	mkdir -p $(TEST_OBJ_DIR)

# 🧪 Construction des tests
test_runner: $(OBJ_WITHOUT_MAIN) $(TEST_OBJ_FILES) $(TEST_RUNNER_OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJ_WITHOUT_MAIN) $(TEST_OBJ_FILES) $(TEST_RUNNER_OBJ) $(LDFLAGS) -o $@
	@echo "✅ Construction des tests terminée !"

# 🧪 Exécution des tests
test: test_runner
	./test_runner

# 🧪 Reconstruction et exécution des tests
retest: clean test_runner test

.PHONY: all clean fclean re run test test_runner retest