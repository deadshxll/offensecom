SRC_FILES=$(find ./src/ -name '*.c')

mkdir build
gcc -o build/offensecom $SRC_FILES -lncurses
if [ $? -eq 0 ]; then
	chmod +x build/offensecom
	build/offensecom
fi
