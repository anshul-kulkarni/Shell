# Shell

A Linux Terminal written in C

# Features:

- Supports history of previous commands executed and can access them using up-down arrow keys.
- Can support multiple as well as single arguments.
- PATH being handled.
- Can exit by pressing Control+D or typing exit.
- PS1 command implemented.

# Current issues:

- Doesn't exit on time by typing 'exit' or pressing Control+D.
- Takes 1 step more to start working normally after using the PS1 command.
- Only known to work on Linux.

# Clone the repository:
```
git clone https://github.com/anshul-kulkarni/Shell.git
```

# Move to the folder:
```
cd Shell
```

# Compile the code:
```
gcc shell.c -lreadline
```
# Run the code:
```
./a.out
```
