
concrete: src/concrete.c	
	$(CC) -g src/concrete.c src/appendbuffer.c src/rowoperations.c src/fileio.c src/definestruct.c src/inputfile.c src/outputfile.c src/terminal.c -o concrete -Wall -Wextra -pedantic -std=c99

# -Wall stands for "all warnings", has compiler warn when code could be bad or questionable
# -Wextra and -pednatic turn on more warnings
