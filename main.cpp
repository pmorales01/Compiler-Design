#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

// look for these terminals when separating terminals from other terminals or tokens
const char TOKENIZE_TERMINALS[10] = {';', ',', ':', '(', ')', '=', '*', '/', '+', '-'};

// use when looking for a row # in get_box()
const string STATES[23] = {"P", "I", "X", "A", "B", "J", "C", "S", "K", "U", "W", "M", "Y", "E", "Q", "T", "R", "F", "N", "D", "G", "H", "L"};

// use when looking for a col # in get_box()
const string TERMINALS[31] = {
  "program", "var", "begin", "end.", "integer", "display", "\"value=\"", "p", "q", "r", "s", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ";",
  ",", ":", "(", ")", "=", "*", "/", "+", "-"};

// list of reserved words
const string RESERVED_WORDS[6]={"program", "var", "begin", "end.", "integer", "display" };

const string PARSING_TABLE[23][31] = {
        {"programI;varAbeginSend.", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank","blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank",
                "blank", "blank", "blank", "blank", "blank", "blank", "blank"},
        {"blank", "blank", "blank", "blank", "blank", "blank", "blank", "LX", "LX", "LX", "LX", "blank", "blank", "blank", "blank", "blank","blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank",
                "blank", "blank", "blank", "blank", "blank", "blank", "blank"},
        {"blank", "blank", "blank", "blank", "blank", "blank", "blank", "LX", "LX", "LX", "LX", "HX", "HX", "HX", "HX", "HX","HX", "HX", "HX", "HX", "HX", "\u03BB", "\u03BB", "\u03BB",
                "blank", "\u03BB", "\u03BB", "\u03BB", "\u03BB", "\u03BB", "\u03BB"},
        {"blank", "blank", "blank", "blank", "blank", "blank", "blank", "B:C;", "B:C;", "B:C;", "B:C;", "blank", "blank", "blank", "blank", "blank","blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank",
                "blank", "blank", "blank", "blank", "blank", "blank", "blank"},
        {"blank", "blank", "blank", "blank", "blank", "blank", "blank", "IJ", "IJ", "IJ", "IJ", "blank", "blank", "blank", "blank", "blank","blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank",
                "blank", "blank", "blank", "blank", "blank", "blank", "blank"},
        {"blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank","blank", "blank", "blank", "blank", "blank", "blank", ",B", "\u03BB",
                "blank", "blank", "blank", "blank", "blank", "blank", "blank"},
        {"blank", "blank", "blank", "blank", "integer", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank","blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank",
                "blank", "blank", "blank", "blank", "blank", "blank", "blank"},
        {"blank", "blank", "blank", "blank", "blank", "UK", "blank", "UK", "UK", "UK", "UK", "blank", "blank", "blank", "blank", "blank","blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank",
                "blank", "blank", "blank", "blank", "blank", "blank", "blank"},
        {"blank", "blank", "blank", "\u03BB", "blank", "S", "blank", "S", "S", "S", "S", "blank", "blank", "blank", "blank", "blank","blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank",
                "blank", "blank", "blank", "blank", "blank", "blank", "blank"},
        {"blank", "blank", "blank", "blank", "blank", "W", "blank", "Y", "Y", "Y", "Y", "blank", "blank", "blank", "blank", "blank","blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank",
                "blank", "blank", "blank", "blank", "blank", "blank", "blank"},
        {"blank", "blank", "blank", "blank", "blank", "display(M", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank","blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank",
                "blank", "blank", "blank", "blank", "blank", "blank", "blank"},
        {"blank", "blank", "blank", "blank", "blank", "blank", "\"value=\",I);", "I);", "I);", "I);", "I);", "blank", "blank", "blank", "blank", "blank","blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank",
                "blank", "blank", "blank", "blank", "blank", "blank", "blank"},
        {"blank", "blank", "blank", "blank", "blank", "blank", "blank", "I=E;", "I=E;", "I=E;", "I=E;", "blank", "blank", "blank", "blank", "blank","blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank",
                "blank", "blank", "blank", "blank", "blank", "blank", "blank"},
        {"blank", "blank", "blank", "blank", "blank", "blank", "blank", "TQ", "TQ", "TQ", "TQ", "TQ", "TQ", "TQ", "TQ", "TQ","TQ", "TQ", "TQ", "TQ", "TQ", "blank", "blank", "blank",
                "TQ", "blank", "blank", "blank", "blank", "TQ", "TQ"},
        {"blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank","blank", "blank", "blank", "blank", "blank", "\u03BB", "blank", "blank",
                "blank", "\u03BB", "blank", "blank", "blank", "+TQ", "-TQ"},
        {"blank", "blank", "blank", "blank", "blank", "blank", "blank", "FR", "FR", "FR", "FR", "FR", "FR", "FR", "FR", "FR","FR", "FR", "FR", "FR", "FR", "blank", "blank", "blank",
                "FR", "blank", "blank", "blank", "blank", "FR", "FR"},
        {"blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank","blank", "blank", "blank", "blank", "blank", "\u03BB", "blank", "blank",
                "blank", "\u03BB", "blank", "*FR", "/FR", "\u03BB", "\u03BB"},
        {"blank", "blank", "blank", "blank", "blank", "blank", "blank", "I", "I", "I", "I", "N", "N", "N", "N", "N","N", "N", "N", "N", "N", "blank", "blank", "blank",
                "(E)", "blank", "blank", "blank", "blank", "N", "N"},
        {"blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "GHD", "GHD", "GHD", "GHD", "GHD","GHD", "GHD", "GHD", "GHD", "GHD", "blank", "blank", "blank",
                "blank", "blank", "blank", "blank", "blank", "GHD", "GHD"},
        {"blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "HD", "HD", "HD", "HD", "HD","HD", "HD", "HD", "HD", "HD", "\u03BB", "blank", "blank",
                "blank", "\u03BB", "blank", "\u03BB", "\u03BB", "\u03BB", "\u03BB"},
        {"blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "\u03BB", "\u03BB", "\u03BB", "\u03BB", "\u03BB","\u03BB", "\u03BB", "\u03BB", "\u03BB", "\u03BB", "blank", "blank", "blank",
                "blank", "blank", "blank", "blank", "blank", "+", "-"},
        {"blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank", "0", "1", "2", "3", "4","5", "6", "7", "8", "9", "blank", "blank", "blank",
                "blank", "blank", "blank", "blank", "blank", "blank", "blank"},
        {"blank", "blank", "blank", "blank", "blank", "blank", "blank", "p", "q", "r", "s", "blank", "blank", "blank", "blank", "blank","blank", "blank", "blank", "blank", "blank", "blank", "blank", "blank",
                "blank", "blank", "blank", "blank", "blank", "blank", "blank"}};

// an indent is 11 spaces long
const int INDENT_SIZE = 11;

/*
 * Reads lines from a text file. Each line is stored as a string in a
 * vector.
 * @param file_name Name of a file to read.
 * @return vector of strings that represent lines read.
*/
vector<string> getLines(string file_name) {
  string line;

  vector<string> lines;

  ifstream file;

  file.open(file_name);

  // read lines from a file
  if (file.is_open()) {
    while (getline(file, line)) {
      if (!line.empty()) { /* line is not a blank line */
         lines.push_back(line);
      }
    }
  }

  file.close();

  return lines;
}

/*
 * Writes strings to a text file. Each string represents a line to write.
 * @param lines Strings representing lines to be written, file Name of
 * file to write to, print Default true to print written lines
*/
void write_lines_to_file(vector<string> lines, string file, int print = true) {
  ofstream outfile;

  outfile.open(file);

  for (int i = 0; i < lines.size(); ++i) {
    if (print) {
    cout << lines[i] << endl;
    }
    if (i != lines.size() - 1) {
      outfile << lines[i] << '\n';
    } else { /* last line to write */
      outfile << lines[i];
    }
  }

  outfile.close();
}

/*
 * Separates tokens joined by the terminals in TOKENIZE_TERMINALS array.
 * For example, "(p2q+2*pr)" is separated to become  "( p2q + 2 * pr )"
 * @param[in, out] word A token from a line
*/
void separate_tokens(string &word) {
  int i = 0; // tracks the current char to be looked at

  size_t val_pos = word.find("\"value=\"");

  // iterate through the token and look for a terminal from TOKENIZE_TERMINALS
  for (; i < word.length(); ++i) {
    for (int j = 0; j < sizeof(TOKENIZE_TERMINALS) / sizeof(char); ++j) {
        if (word[i] == '"') { /* "value=" found */
          i = i + 7; // move to the char after the closing "
          break;
        }
        if (word[i] == TOKENIZE_TERMINALS[j]) { /* terminal found */
            // add a space before and after the terminal
            string replacement = string(" ") + word[i] + " ";
            word = word.replace(i, 1, replacement);
            // change the current position to 2 indices after the current one
            i = i + 2;
            break;
        }
    }
  }
}


/*
 * Removes any extra spaces between tokens.
 * @param line String representing a line in a file.
 * @return line with no extra spaces
*/
string remove_extra_spaces(string line) {
  stringstream ss(line);

  string new_line, word, final_line;

  // remove empty spaces as well as separate tokens from other tokens
  while (ss >> word) {
    separate_tokens(word);

    // add space between each token
    new_line = new_line + " " + word;
  }

  stringstream ss2(new_line);

  // remove any extra spaces after separating tokens
  while (ss2 >> word) {
    final_line = final_line + " " + word;
  }

  final_line.erase(0, 1);

  return final_line;
}

/*
 * Concatenates leading spaces to the front of a line to represent the
 * indentation of a line. Also removes any extra and leading spaces in the line.
 * @param[in, out] line Line in a file
*/
void indent_line(string &line) {
  // find and remove any leading spaces
  size_t first_non_space = line.find_first_not_of(" ");

  line = line.substr(first_non_space);

  // remove extra spaces
  line = remove_extra_spaces(line);

  // add spaces that represent indentation
  line = string(INDENT_SIZE, ' ') + line;
}

/*
 * Prepares the lines of a file to be tokenized by removing comment lines,
 * blank lines, extra spaces, and formatting the lines to be aligned.
 * @param lines Vector of strings representing lines in a file.
*/
void tokenize(vector<string> lines) {
  int line_count = lines.size();

  for (int j = 0; j < line_count; j++) {
    int line_len = lines[j].length();

    for (int i = 0; i < lines[j].length(); i++) {
      // comment removing
      size_t comment1 = lines[j].find("//");

      if (comment1 != string::npos) {

        size_t comment2 = lines[j].find("//", comment1 + 2);
        // remove whole line if // is at end of line or only one //

        if (comment2 == string::npos) {
          if (lines[j].find_first_not_of(' ', comment1 + 2) == string::npos) {
            // remove lines that end with // but don't start with //
            lines.erase(lines.begin() + j);
            line_count -= 1;
          } else { /* remove comment that ends with the return key*/
            lines[j].erase(comment1, lines[j].length());
          }
        }
        // else remove // // and everything inside them
        else {
          lines[j].erase(comment1, comment2);
        }
      }

      // remove blank lines if line is only spaces
      if (lines[j].find_first_not_of(' ') == string::npos) {
        lines.erase(lines.begin() + j);
      }
    }
  }

  // format and remove extra spaces after removing comments and blank lines
  for (int i = 0; i < lines.size(); ++i) {
    // remove leading and extra spaces and indent line
    indent_line(lines[i]);
  }

  write_lines_to_file(lines, "finalp2.txt");
}

/*
 * Reads and extracts the tokens from a file.
 * @param file_name Name of file to read and extract tokens from
 * @return vector of tokens
*/
vector<string> getTokens(string file_name) {
  string line, word;

  vector<string> tokens;

  ifstream file;

  file.open(file_name);

  // read lines from a file
  if (file.is_open()) {
    while (getline(file, line)) {
      if (!line.empty()) { /* line is not a blank line */
        stringstream ss(line);
        // for each line, push all the tokens into a vector
        while (ss >> word) {
          tokens.push_back(word);
        }
      }
    }
  }

  file.close();

  return tokens;
}

/*
 * Prints out the current contents of the stack.
 * @param stack Stack containing terminals and non-terminals
*/
void print_stack(stack<string> stack) {
    std::stack<string> new_stack;

    cout << "stack: ";

    // store all the symbols in a new stack in reverse order
    while (!stack.empty()) {
        new_stack.push(stack.top());
        stack.pop();
    }

    // print the stack
    while (!new_stack.empty()) {
        {
            cout << new_stack.top() << " ";
            new_stack.pop();
        }
    }

    cout << endl;
}

/*
 * Checks if a token is a reserved word, identifier, or number.
 * @param token Terminal or non-terminal token
 * @return "reserved" for reserved words, "identifier" for identifiers,
 * "number" for numbers, and "other" otherwise
*/
string get_token_type(string token) {
  // check if the first char is a number
    if (isdigit(token[0])) {
      // if so check if the rest of the token are numbers
      for (int i = 1; i < token.length(); i++) {
        // if not exit function
        if (!isdigit(token[i])) {
          return "other";
        }
      }
      // if token is a number return "number"
      return "number";
    } else {
      // check if token is a reserved word
      for (int i = 0; i < sizeof(RESERVED_WORDS) / sizeof(RESERVED_WORDS[0]);
          i++) {
        if (token == RESERVED_WORDS[i]) { /* token is a reserved word */
          return "reserved";
        }
      }

      // checks if token is an identifier
      for (int i = 0; i < token.length(); i++) {
        if (token[i] != 'p' && token[i] != 'q' && token[i] != 'r' &&
            token[i] != 's' && !isdigit(token[i])) {
          return "other";
        }
      }

      // if so mark token as an identifier
      return "identifier";
    }

    // token is neither a reserved word, identifier, or number
    return "other";
}


/* Returns the value of the box at the specified index of the parsing table
 * @param state Top of stack, terminal Current token from input,
 * stack The current stack being used, tokens_left Number of tokens left to parse
 * @return empty string if symbols were successfully pushed to the stack,
 * "blank" if the box is blank, "DNE" if the terminal to look for is an
 * identifier or number, or "reject" if box's value could not be retrieved
*/
string get_box(string state, string terminal, stack<string>& stack, int tokens_left) {
    int row = -1, col = -1;

    // std::cout << "[" << state << " , " << terminal << "]" << endl;

   // look for state (row)
    for (int i = 0; i < sizeof(STATES) / sizeof(STATES[0]); ++i) {
      if (STATES[i] == state) {
        row = i;
      }
    }

    // look for terminal (col)
    for (int i =0; i < sizeof(TERMINALS) / sizeof(TERMINALS[0]); ++i) {
      if (TERMINALS[i] == terminal) {
        col = i;
      }
    }

    if (row == -1) { /* state not found (token missing) */
        if (state == "var") {
          cout << "var is expected" << endl;
        } else if (state == "begin") {
          cout << "begin is expected" << endl;
        } else if (state == "(" || terminal == ")") {
          cout << "( is missing" << endl;
        } else if (state == ")") {
          cout << ") is missing" << endl;
        } else if (state == ";") {
          cout << "; is missing" << endl;
        } else if (state == ",") {
          cout << ", is missing" << endl;
        } else {
          cout << "REJECTED" << endl;
        }

        return "blank";
    }

    //terminal is an identifier or number
    if (col == -1) {
      // beginning of a write or assign statement
      if (state == "K" && (get_token_type(terminal) == "other" || terminal == "")) {
        // if tokens left is 0, then end. is expected
        if (tokens_left <= 0) {
          if (terminal == "end") { /* . is missing from "end." */
            cout << ". is missing" << endl;
          } else { /* end. is missing or spelled wrong */
            cout << "end. is expected" << endl;
          }
        } else { /* display is spelled wrong */
          cout << "display is expected" << endl;
        }
      } else if (state == "C") { /* integer is spelled wrong */
        cout << "integer is expected" << endl;
      } else if (get_token_type(terminal) == "number" || get_token_type(terminal) == "identifier") {
        stack.push(state); // push the state back to stack
        return "DNE"; // token is identifier or number longer than 2 chars
      } else if (state == "P") { /* program spelled wrong */
        cout << "program is expected" << endl;
      } else if (state == "M") {
        cout << "REJECTED" << endl;
      } else {
        cout << "; is missing" << endl;
      }

      return "reject";
    }

    // push these symbols to the stack (only for these specific boxes)
    if (row == 0 && col == 0) { /* [P, program]*/
      stack.push("end.");
      stack.push("S");
      stack.push("begin");
      stack.push("A");
      stack.push("var");
      stack.push(";");
      stack.push("I");
      stack.push("program");
    } else if (row == 6 && col == 4) { /* [C, integer]*/
      stack.push("integer");
    } else if (row == 10 && col == 5) { /* [W, display] */
      stack.push("M");
      stack.push("(");
      stack.push("display");
    } else if (row == 11 && col == 6) { /* [M, "value="] */
      stack.push(";");
      stack.push(")");
      stack.push("I");
      stack.push(",");
      stack.push("\"value=\"");
    } else {
      string box_val = PARSING_TABLE[row][col];

      if (box_val == "blank") {
        if (state == "P") { /* program is missing or spelled wrong */
          cout << "program is expected" << endl;
        } else if (state == "E") {
          cout << "REJECTED" << endl;
        } else if (state == "C") { /* integer is missing */
          if (terminal == ";") {
            cout << "integer is expected" << endl;
          } else {
            cout << "REJECTED" << endl;
          }
        } else if (state == "K") { /* display is missing */
          if (terminal == ";") {
            cout << "REJECTED" << endl;
          } else {
            cout << "display is expected" << endl;
          }
        } else if (terminal == "var") {
          cout << "; is missing" << endl;
        } else if (state == "D") { /* terminal expects a digit */
          if (terminal == "p" || terminal == "q" || terminal == "r" || terminal == "s") {
            cout << "REJECTED" << endl;
          } else {
            cout << "; is missing" << endl;
          }
        } else if (state == "X") { /* identifier followed by reserved word or ( */
          if (terminal == "integer") { /* identifier followed by integer */
            cout << ": is missing" << endl;
          } else {
            cout << "; is missing" << endl;
          }
        } else if (state == "W") {
          cout << "; is missing" << endl;
        } else if (terminal == "display") {
          cout << "; is missing " << endl;
        } else if (state == "J") {
          cout << ": is missing" << endl;
        } else if (state == "R") {
          cout << "; is missing" << endl;
        } else {
          cout << "REJECTED" << endl;
        }

        return "blank";
      }

      if (box_val != "\u03BB") { /* box is not lambda or blank */
        // push symbols from parsing table to stack
        for (int i = box_val.length() - 1; i >= 0; --i) {
            string c(1,box_val[i]);
            stack.push(c);
        }
      }
    }

    // print_stack(stack);
    return "";
}

/*
 * Checks if an identifier has been defined in the program.
 * @param variable Identifier to check, identifiers Defined identifiers
 * @return true if identifier has been defined, false if not defined
*/
bool var_is_defined(string variable, vector<string>& identifiers) {
  for (int i = 0; i < identifiers.size(); i++) {
    if (variable == identifiers[i]) {
      return true;
    }
  }
  return false;
}

/*
 * Traces the tokens from a tokenized input file by using a predictive
 * parsing table.
 * @param input Tokens to be parsed using the predictive parsing table
 * @return "reject" if program is rejected, "Accept" if program is accepted
*/
string parse(vector<string> input) {
    stack<string> stack;
    //used for names in input
    string remainder;

    //used to go through input vector
    int i = 0;

    // tracks if the read token has been parsed
    bool token_parsed = false;

    // tracks if the read token is currently being parsed
    bool parsing_token = false;

    // tracks the read token's data type
    string parsed_token_type;

    // tracks if variables(identifiers) are currently being declared
    bool declaring_var;

    // tracks the declared identifiers in the entire program
    vector<string> identifiers;

    //initialize token
    string token = "\0";

    //initial push
    stack.push("P");

    while (!stack.empty()) {
      // a symbol is a non-terminal or terminal from the grammar and stack
      string symbol = stack.top();

      stack.pop();

      // cout << "pop: " << symbol << endl;

      // read a token from input string since one hasn't been read
      if(token=="\0") {
        token=input[i];
        i++; // increment index to read next token

        // cout << "read: " << token << endl;

        // if "var" is read, then set declaring_var to true to indicate
        // that identifiers will be declared
        if (token == "var") {
          declaring_var = true;
        } else if (token == ":") {
          // when program reads  ":", then done declaring identifiers
          declaring_var = false;
        }

        // if declaring variables (identifiers), add the identifier to the list
        // of declared identifiers
        if (declaring_var && get_token_type(token) == "identifier") {
          identifiers.push_back(token);
        }

        // tracks if token is a single character number or identifier
        if ((get_token_type(token) == "number" || get_token_type(token) == "identifier") && token.length() == 1 && !token_parsed) {
          // cout << "tracking token " << token << endl;
          // reject if identifier has not been declared (unknown)
          if (!var_is_defined(token, identifiers) && identifiers.size() > 0 && get_token_type(token) == "identifier") {
            cout << "unknown identifier " << token << endl;
            return "reject";
          }
          parsing_token = true; // set to true since currently parsing token
          parsed_token_type = get_token_type(token); // get the token's type
        }

        if (token_parsed) { /* if previous token is done parsing */
          string current_token_type = get_token_type(token);

          // missing operator or "," between 2 identifiers
          if (current_token_type == "identifier" && parsed_token_type == "identifier") {
            // currently declaring variables, so "," is missing
            if (declaring_var) {
              cout << ", is missing" << endl;
            } else { /* not declaring variables, so an operator is missing */
              cout << "; is missing" << endl;
            }
            return "reject";
          }

          // if an operator + - / * is missing between 2 numbers, assume its
          //  the end of a statement and a ; is missing
          if (current_token_type == "number" && parsed_token_type == "number") {
            cout << "; is missing" << endl;
            return "reject";
          }

          // operator missing between an identifier and number
          if (current_token_type == "identifier" && parsed_token_type == "number" ) {
            cout << "; is missing" << endl;
            return "reject";
          }

          // operator missing between a number and identifier
          if (current_token_type == "number" && parsed_token_type == "identifier") {
            cout << "; is missing" << endl;
            return "reject";
          }

          token_parsed = false; // reset the flags to parse a new token
          parsed_token_type = "";
        }
      }

      if (symbol == token && token != "\0") { /* match found */
        // reset the token to be empty
        token = "\0";

        // cout << symbol << " match\t"<<endl;
        // print_stack(stack);

        if (symbol == "end.") { /* end of input string reached */
            cout << "ACCEPTED\n" << endl;
            return "Accept";
        }

        // if the remainder string isn't empty
        // token will become the next char in remainder
        // and then reduce remainder by 1
        if (!remainder.empty()) {
            token = string(1,remainder[0]);
            remainder =  remainder.substr(1);
            // cout << "token = " << token << " remainder = " << remainder << endl;
        } else if (remainder.empty() && parsing_token) { /*done parsing token*/
          token_parsed = true;
          parsing_token = false;
        }

        continue;
      }

      // get the value of the box from the parsing table
      string box_val = get_box(symbol, token, stack, input.size() - i);

      /* parsing table returned blank */
      if (box_val == "blank" || box_val == "reject") {
          // cout << "REJECTED\n" << endl;
          return "Reject";
      }

      //get box will return DNE if the token is not on the list
      //token is an identifier or number longer than 1 character
      //It will take the name one character at a time allowing for tracing
      if(box_val=="DNE"){
        // cout << "DNE" << endl;

        // checks if identifier has been declared, else return error
        if (!var_is_defined(token, identifiers) && identifiers.size() > 0 && get_token_type(token) == "identifier") {
          cout << "unknown identifier " << token << endl;
          return "Reject";
        }

        //take the first letter of the current token and save it as token
        //and save the rest in remainder
        parsed_token_type = get_token_type(token);
        string c(1,token[0]);
        remainder =  token.substr(1);
        token=c;
        parsing_token = true;

        //used to see output can remove
        // cout <<" token is a name read as "<<c<<endl;
        // cout << "remainder = " << remainder << endl;
        // print_stack(stack);
        continue;
      }
    }
  return "reject";
}


/*  Goes through the list of parsed tokens to translate them to a C++ program.
  * @param tokens Vector of (accepted) parsed tokens to translate
*/
void translate(vector<string> tokens) {
  // vector containing translated C++ lines
  vector<string> translated_lines;

  // program_name name of the program (C++ file)
  // stat_type tracks the type of statement (write or assign)
  string program_name, stat_type;

  int i = 0; // tracks current token

  string line = "\t"; // init line with indent

  if (tokens[i] == "program") { /* initialize C++ file */
    i++; // increment to go to next token (program name)
    program_name = tokens[i] + ".cpp";
    i++; // go to ; (program name has been defined)
    translated_lines.push_back("#include <iostream>");
    translated_lines.push_back("using namespace std;");
    translated_lines.push_back("int main()");
    translated_lines.push_back("{");
    i++; // increment to go to var
  }

  if (tokens[i] == "var") { /* identifiers are defined */
    line = line +  "int ";
    i++; // increment to go to first identifier

    // search for identifiers that are currently being defined
    while (tokens[i] != ":" && i < tokens.size()) {
      line = line + tokens[i] + " "; // append tokens to line
      i++;
    }
    line = line + ";";
    translated_lines.push_back(line); // push translated line to vector
    i += 3; // increment to begin
  }

  line = "\t"; // reset line

  if (tokens[i] == "begin") { /* translate statements */
    i++; // increment to 1st statement

    // while end. is not reached, translate each statement
    while (tokens[i] != "end." && i < tokens.size()) {
      line = "\t"; // reset the line

      // determine if statement is a write or assign statement
      if (tokens[i] == "display") {
          stat_type = "write";
          i++;
      } else if (i + 1 < tokens.size() && tokens[i + 1] == "=") {
          stat_type = "assign";
      }

      if (stat_type == "assign") {
        // push tokens to line until ; is reached
        while (i < tokens.size()) {
          // cout << tokens[i] << endl;
          line = line + tokens[i] + " ";
          if (tokens[i] == ";") {
            i++;
            break;
          }
          i++;
        }
      } else if (stat_type == "write") {
        line = line + "cout << " + " ";
        // push tokens to line until ; is reached
        while (i < tokens.size()) {
          // ignore ( ) , characters
          if (tokens[i] != "(" && tokens[i] != ")" && tokens[i] != "," && tokens[i] != ";") {
            line = line + tokens[i] + " << " + " " ;
          }

          if (tokens[i] == ";") {
            line = line + "endl " + tokens[i];
            i++;
            break;
          }
          i++;
        }

      }
      // push lines to vector of translated C++ lines
      translated_lines.push_back(line);
    }
  }

  if (tokens[i] == "end.") { /* end. is reached */
    translated_lines.push_back("\treturn 0;");
    translated_lines.push_back("}");
  }

  // create a C++ file named after program_name
  write_lines_to_file(translated_lines, program_name, false);
}

int main() {
  // read lines from the file
  vector<string> lines = getLines("finalp1.txt");

  // format the file
  tokenize(lines);

  // get tokens from the new file
  vector<string> tokens = getTokens("finalp2.txt");

  string trace_results = parse(tokens); // parse the tokens

  // cout << "Results = " << trace_results << endl;

  if (trace_results == "Accept") { /* accepted, so translate to C++ */
    translate(tokens);
  }
}
