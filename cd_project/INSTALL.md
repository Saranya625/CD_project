# Installation and Usage Guide

## System Requirements

This parser requires the following tools to be installed on your system:

1. **Flex** (Fast Lexical Analyzer Generator)
2. **Bison** (GNU Parser Generator)
3. **GCC** (GNU Compiler Collection)

## Installation Instructions

### Ubuntu/Debian Linux
```bash
sudo apt-get update
sudo apt-get install flex bison gcc build-essential
```

### Fedora/RHEL/CentOS
```bash
sudo yum install flex bison gcc
```

### macOS (using Homebrew)
```bash
brew install flex bison gcc
```

### Windows (using MinGW or Cygwin)
Install MinGW or Cygwin and use their package managers to install flex, bison, and gcc.

## Building the Compiler

Once you have the required tools installed:

1. Navigate to the project directory:
```bash
cd /path/to/project
```

2. Build the compiler:
```bash
make
```

This will:
- Generate `parser.tab.c` and `parser.tab.h` from `parser.y` using Bison
- Generate `lex.yy.c` from `lexer.l` using Flex
- Compile all generated C files into an executable named `compiler`

## Running the Compiler

### Test with provided examples:
```bash
./compiler < test_bubble_sort.c
./compiler < test_features.c
./compiler < test_globals.c
```

### Test with your own program:
```bash
./compiler < your_program.c
```

### Redirect output to a file:
```bash
./compiler < your_program.c > output.txt
```

## Expected Output

### For a valid program:
```
Starting parser...
Valid program parsed successfully!
Parsing completed successfully.
```

### For an invalid program:
```
Starting parser...
Parse error at line X: syntax error
Parsing failed.
```

### For lexical errors:
```
Lexical Error at line X: [character]
```

## Cleaning Build Files

To remove all generated files:
```bash
make clean
```

This removes:
- `compiler` (executable)
- `lex.yy.c` (generated lexer)
- `parser.tab.c` and `parser.tab.h` (generated parser)

## Troubleshooting

### Problem: "command not found" when running make
**Solution**: Install the `make` utility:
```bash
sudo apt-get install build-essential  # Ubuntu/Debian
sudo yum install make                 # Fedora/RHEL
```

### Problem: "bison: command not found"
**Solution**: Install bison as shown in the installation section above

### Problem: "flex: command not found"
**Solution**: Install flex as shown in the installation section above

### Problem: Compilation errors about undefined references
**Solution**: Make sure you're linking with the flex library:
```bash
gcc -o compiler parser.tab.c lex.yy.c -lfl
```

If `-lfl` doesn't work, try `-ll`:
```bash
gcc -o compiler parser.tab.c lex.yy.c -ll
```

### Problem: Parser doesn't recognize valid syntax
**Solution**: 
1. Check that your input file matches the language specification
2. Ensure all statements end with semicolons
3. Verify braces are properly matched
4. Make sure the program starts with `int main() {`

## Testing Your Programs

### Create a test file:
```c
int main() {
    int x;
    x = 5;
    print("Value: ", x);
    return 0;
}
```

### Save it as `test.c` and run:
```bash
./compiler < test.c
```

## Understanding Parser Output

The parser validates:
1. **Lexical correctness**: All tokens are valid
2. **Syntactic correctness**: Token sequence follows grammar rules
3. **Structural correctness**: Braces, parentheses match properly

The parser does NOT validate:
1. Semantic correctness (type checking)
2. Variable usage before declaration
3. Array bounds
4. Type compatibility in expressions

## Next Steps

After successfully parsing, you would typically:
1. Build a symbol table
2. Perform semantic analysis
3. Generate intermediate code
4. Optimize the code
5. Generate target code

This parser forms the foundation for these subsequent compiler phases.
