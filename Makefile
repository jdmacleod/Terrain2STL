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
	
ziptile: ## Build the celevstl-ziptile executable
	gcc src/elevstl-ziptile.c src/STLWriter.c src/elevation-ziptile.c src/readzip.c src/tiles.c -o celevstl-ziptile -lm -lzip -lz

test-zip: zip ## test the celevstl-zip executable
	./celevstl-zip 44.1928 -69.0851 40 40 1.7 0 1 3 1 test-zip.stl

readzip: ## Build the readzip test executable
	gcc src/readzip_main.c src/readzip.c -o readzip -lzip -lz

readziptile: ## Build the readziptile test executable
	gcc src/readziptile_main.c src/readzip.c -o readziptile -lzip -lz

tiles: ## Build the tiles test executable
	gcc src/tiles_main.c src/tiles.c -o tiles

tileszip: ## Build the tileszip test executable
	gcc src/tileszip_main.c src/tiles.c src/readzip.c -o tileszip -lzip -lz

server-dirs:  ## create server output directories
	mkdir -p logs
	mkdir -p stls

run-server: default server-dirs
	npm install
	node terrainServer.js