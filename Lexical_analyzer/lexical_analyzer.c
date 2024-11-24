#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LEXEMES 100
#define MAX_LINE_LENGTH 256
#define MAX_SYMBOLS 100

// Structure to store lexeme information
struct Lexeme
{
    int line_no;
    char lexeme[100];
    char token[50];
    char token_value[50]; // Stores index as token value
};

// Structure to store symbol table entry
struct Symbol
{
    int index;
    char symbol[100];
};

// Keywords for C
const char *keywords[] = {"int", "float", "char", "if", "else", "while", "for", "return", "void"};
const int num_keywords = 9;

// Operators for C
const char *operators[] = {"+", "-", "*", "/", "=", "==", "!=", "<", ">", "<=", ">="};
const int num_operators = 11;

// Delimiters
const char *delimiters[] = {"(", ")", "{", "}", ";", ","};
const int num_delimiters = 6;

// Static array to store lexemes and symbol table
struct Lexeme lexeme_table[MAX_LEXEMES];
int lexeme_count = 0;

struct Symbol symbol_table[MAX_SYMBOLS];
int symbol_count = 0;

// Check if a string is a keyword and return its index
int is_keyword(const char *str)
{
    for (int i = 0; i < num_keywords; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
            return i; // Return index of keyword
    }
    return -1;
}

// Check if a string is an operator and return its index
int is_operator(const char *str)
{
    for (int i = 0; i < num_operators; i++)
    {
        if (strcmp(str, operators[i]) == 0)
            return i; // Return index of operator
    }
    return -1;
}

// Check if a string is a delimiter and return its index
int is_delimiter(const char *str)
{
    for (int i = 0; i < num_delimiters; i++)
    {
        if (strcmp(str, delimiters[i]) == 0)
            return i; // Return index of delimiter
    }
    return -1;
}

// Add a symbol to the symbol table if it doesn't exist and return its index
int add_symbol(const char *symbol)
{
    // Check if the symbol already exists in the symbol table
    for (int i = 0; i < symbol_count; i++)
    {
        if (strcmp(symbol_table[i].symbol, symbol) == 0)
        {
            return i; // Return the index if the symbol is already in the table
        }
    }

    // Add a new symbol to the table
    strcpy(symbol_table[symbol_count].symbol, symbol);
    symbol_table[symbol_count].index = symbol_count;
    symbol_count++;

    return symbol_count - 1; // Return the index of the new symbol
}

// Add a lexeme to the lexeme table
void add_lexeme(int line_no, const char *lex, const char *token, const char *value)
{
    lexeme_table[lexeme_count].line_no = line_no;
    strcpy(lexeme_table[lexeme_count].lexeme, lex);
    strcpy(lexeme_table[lexeme_count].token, token);
    strcpy(lexeme_table[lexeme_count].token_value, value);
    lexeme_count++;
}

// Lexical analysis function
void lexical_analyzer(FILE *file)
{
    char line[MAX_LINE_LENGTH];
    int line_no = 0;
    int paren_count = 0;

    while (fgets(line, sizeof(line), file))
    {
        line_no++;
        char *token = strtok(line, " \t\n");

        while (token != NULL)
        {
            int index;

            // Check for keywords
            if ((index = is_keyword(token)) != -1)
            {
                char index_str[10];
                sprintf(index_str, "%d", index); // Convert index to string
                add_lexeme(line_no, token, "KEYWORD", index_str);
            }
            // Check for operators
            else if ((index = is_operator(token)) != -1)
            {
                char index_str[10];
                sprintf(index_str, "%d", index); // Convert index to string
                add_lexeme(line_no, token, "OPERATOR", index_str);
            }
            // Check for delimiters
            else if ((index = is_delimiter(token)) != -1)
            {
                char index_str[10];
                sprintf(index_str, "%d", index); // Convert index to string
                add_lexeme(line_no, token, "DELIMITER", index_str);
                if (strcmp(token, "(") == 0)
                    paren_count++;
                if (strcmp(token, ")") == 0)
                    paren_count--;
            }
            // Check for numeric literals
            else if (isdigit(token[0]))
            {
                add_lexeme(line_no, token, "Constant", token);
            }
            // Otherwise, it's an identifier
            else
            {
                int symbol_index = add_symbol(token); // Add identifier to symbol table
                char index_str[10];
                sprintf(index_str, "%d", symbol_index); // Convert index to string
                add_lexeme(line_no, token, "IDENTIFIER", index_str);
            }

            token = strtok(NULL, " \t\n");
        }
    }

    // Error checking: unmatched parentheses
    if (paren_count != 0)
    {
        printf("Error: Unmatched parentheses detected!\n");
    }
}

// Write the lexeme table and symbol table to the output file
void write_lexeme_table_to_file()
{
    FILE *output_file = fopen("output.txt", "w");

    if (output_file == NULL)
    {
        printf("Error: Unable to open output file.\n");
        return;
    }

    // Print lexeme table header with vertical lines
    fprintf(output_file, "+--------+---------------+---------------+---------------+\n");
    fprintf(output_file, "| %-6s| %-13s | %-13s | %-13s |\n", "Line No", "Lexeme", "Token", "Token Value");
    fprintf(output_file, "+--------+---------------+---------------+---------------+\n");

    // Print lexeme table with vertical lines
    for (int i = 0; i < lexeme_count; i++)
    {
        fprintf(output_file, "| %-6d | %-13s | %-13s | %-13s |\n",
                lexeme_table[i].line_no,
                lexeme_table[i].lexeme,
                lexeme_table[i].token,
                lexeme_table[i].token_value);
    }

    // Print table footer
    fprintf(output_file, "+--------+---------------+---------------+---------------+\n\n");

    // Print symbol table header
    fprintf(output_file, "+--------+---------------+\n");
    fprintf(output_file, "| %-6s | %-13s |\n", "Index", "Symbol");
    fprintf(output_file, "+--------+---------------+\n");

    // Print symbol table with vertical lines
    for (int i = 0; i < symbol_count; i++)
    {
        fprintf(output_file, "| %-6d | %-13s |\n", symbol_table[i].index, symbol_table[i].symbol);
    }

    // Print symbol table footer
    fprintf(output_file, "+--------+---------------+\n");

    fclose(output_file);
}

int main()
{
    FILE *file = fopen("input_code.c", "r");
    if (file == NULL)
    {
        printf("Error: Unable to open file.\n");
        return 1;
    }

    // Perform lexical analysis
    lexical_analyzer(file);

    // Write the lexeme table and symbol table to output.txt
    write_lexeme_table_to_file();

    fclose(file);
    printf("Lexical analysis completed. Output written to output.txt\n");
    return 0;
}
