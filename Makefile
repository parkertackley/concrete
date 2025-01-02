concrete: concrete.c	
	$(CC) concrete.c -o concrete -Wall -Wextra -pedantic -std=c99

# -Wall stands for "all warnings", has compiler warn when code could be bad or questionable
# -Wextra and -pednatic turn on more warnings