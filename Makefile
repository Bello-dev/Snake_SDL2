compile:
	@g++ -o prog *.cpp -lm -lSDL2 -lSDL2_ttf -lSDL2_gfx -lSDL2_mixer -lSDL2_image && clear && echo "Executable mis à jour avec succès"

run:
	clear && ./prog 600 600

clean:
	@rm -f prog && clear && echo "Executable supprimé avec succès"
