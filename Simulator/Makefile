# Variables
TASK ?= ~/P1/Simulator
FILE ?= main
SRC = $(TASK)/$(FILE).c
EXEC = $(TASK)/$(FILE)
COMMIT_MSG ?= "Initial commit"
GIT_USERNAME ?= Nehal-2

# Current directory name
REPO_NAME := $(shell basename $(shell pwd))

# Default target: compile the C source files
all: compile

compile:
	@echo "Compiling C files in $(TASK)..."
	@gcc $(SRC) -o $(EXEC)

# Run the compiled executable
run: compile
	@echo "Running $(EXEC)..."
	@./$(EXEC)

# Clean up generated executables
clean:
	@echo "Cleaning up all executables..."
	@find . -type f -executable -exec rm -f {} +

# Edit the specified C file with vim
edit:
	@echo "Opening $(SRC) in vim..."
	@vim $(SRC)

# Git repository target
git:
	@echo "Initializing git repository if not initialized..."
	@git init
	@echo "Setting the remote repository URL..."
	@git remote add origin https://github.com/$(GIT_USERNAME)/$(REPO_NAME).git || echo "Remote origin already exists, skipping..."
	@echo "Setting the branch to main..."
	@git branch -M main

# Git commit target
commit: git
	@git add .
	@echo "Committing changes..."
	@if ! git diff-index --quiet HEAD; then \
		git commit -m $(COMMIT_MSG); \
	else \
		echo "No changes to commit."; \
	fi

# Git push target
push: commit
	@echo "Pushing changes to the main branch..."
	@git push -u origin simulator

# Declare phony targets
.PHONY: all compile run clean edit git commit push
