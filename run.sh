SRC_FILES=$(find ./src/ -name '*.c') # This grabs all the .c files inside the `src` directory

mkdir build 2>/dev/null
gcc -o build/offensecom $SRC_FILES -lncurses
if [ $? -eq 0 ]; then      # Checks if the build is successful
	chmod +x build/offensecom # Sets the output file as executable
	build/offensecom          # Runs the output file
fi
