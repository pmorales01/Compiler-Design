# Compiler-Design

Language Used: C++
Parsing Method: Predictive Parsing

# finalp1.txt
* input file for the compiler
* compiler will remove any comments, blank lines, extra spaces, and leave one space before and after each token (prepare finalp1.txt for parsing)
* output file (file ready for translation) is finalp2.txt

# Grammar
<img width="594" alt="image" src="https://github.com/pmorales01/Compiler-Design/assets/103544215/c480cfdd-7386-4349-94b5-0c979cfeec6f">

# Reserved Words
* program
* var
* begin
* end.
* integer
* display

# Error Detection
         program   is expected (if program is missing or spelled wrong)
         
         var            is expected ( if var is missing or spelled wrong)
         
         begin         is expected (if begin is missing or spelled wrong) 
         
         end.           is expected (if end. is missing or spelled wrong) 
         
         integer      is expected (if integer is missing or spelled wrong)
         
         unknown identifier if variable is not defined
         
         display      is expected ( if display spelled wrong)
         
         ;                 ; is missing
         
         ,                 , is missing
         
         .                 . is missing
         
         (                  ( is missing
         
         )                  ) is missing 

* for all other errors, "REJECTED" will be printed out

# Acceptance
* Upon acceptance by the compiler, the compiler will output finalp2.txt. The program in finalp2.txt will then be translated into a functional C++ program (<name of program>.cpp)

# Commands
1. Compile main.cpp
```
g++ -std=c++17 -o out main.cpp
```

2. Run the executable
```
./out
```

3. Compile the translated C++ program (default is named s2023.cpp)
```
g++ -std=c++17 -o run s2023.cpp
```

4. Run the executable
```
./run
```
