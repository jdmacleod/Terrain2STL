.PHONY: help

help: ## Display this help screen
	@echo
	@echo "Usage:"
	@echo
	@sed -n 's/^\([A-Za-z0-9_.-]*\):.*## \(.*\)$$/\t\1: \2/p' Makefile | sort | column -t -s ':'
	@echo
	
default: ## Build the celevstl executable
	gcc src/elevstl.c src/STLWriter.c src/elevation.c -o celevstl -lm

test: default ## test the celevstl executable
	./celevstl 44.1928 -69.0851 40 40 1.7 0 1 3 1 test.stl

zip: ## Build the celevstl-zip executable
	gcc src/elevstl-zip.c src/STLWriter.c src/elevation-zip.c src/readzip.c -o celevstl-zip -lm -lzip -lz
	
test-zip: zip ## test the celevstl-zip executable
	./celevstl-zip 44.1928 -69.0851 40 40 1.7 0 1 3 1 test-zip.stl

server-dirs:  ## create server output directories
	mkdir -p logs
	mkdir -p stls

run-server: default server-dirs
	npm install
	node terrainServer.js