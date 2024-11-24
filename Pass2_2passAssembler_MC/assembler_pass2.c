#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMBOLS 100
#define MAX_LITERALS 100
#define MAX_CODE_LINES 100

typedef struct
{
    char *name;
    int address;
} Symbol;

typedef struct
{
    char *name;
    int address;
} Literal;

typedef struct
{
    int lc;
    char operation[3];
    int opcode;
    int reg;
    char operandType[2];
    int operandValue;
} IntermediateCode;

Symbol symbol_table[MAX_SYMBOLS];
int symbol_count = 0;

Literal literal_table[MAX_LITERALS];
int literal_count = 0;

IntermediateCode intermediate_code[MAX_CODE_LINES];
int code_line_count = 0;

void read_pass1_output(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Error: Unable to open file %s\n", filename);
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "LC", 2) == 0 || strncmp(line, "Symbol", 6) == 0)
        {
            continue; // Skip header lines
        }

        if (strchr(line, '('))
        {
            sscanf(line, "%d (%[^,], %d) (%[^,], %d)",
                   &intermediate_code[code_line_count].lc,
                   intermediate_code[code_line_count].operation,
                   &intermediate_code[code_line_count].opcode,
                   intermediate_code[code_line_count].operandType,
                   &intermediate_code[code_line_count].operandValue);

            if (strstr(line, "(R,"))
            {
                sscanf(line, "%*d (%*[^,], %*d) (R, %d)",
                       &intermediate_code[code_line_count].reg);
            }
            else
            {
                intermediate_code[code_line_count].reg = -1;
            }

            code_line_count++;
        }
        else
        {
            // Parse symbol or literal table entry
            int index, address;
            char name[20];
            if (sscanf(line, "%d %s %d", &index, name, &address) == 3)
            {
                if (line[0] == 'L')
                { // Literal Table Entry
                    literal_table[index].name = strdup(name);
                    literal_table[index].address = address;
                    literal_count++;
                }
                else
                { // Symbol Table Entry
                    symbol_table[index].name = strdup(name);
                    symbol_table[index].address = address;
                    symbol_count++;
                }
            }
        }
    }

    fclose(file);
}

void generate_machine_code(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Error: Unable to open file %s\n", filename);
        return;
    }

    for (int i = 0; i < code_line_count; i++)
    {
        fprintf(file, "%d ", intermediate_code[i].opcode);

        if (intermediate_code[i].reg != -1)
        {
            // If there is a register involved, print it
            fprintf(file, "%d ", intermediate_code[i].reg);
        }

        if (strcmp(intermediate_code[i].operandType, "S") == 0)
        {
            // Operand is a symbol, print its address
            fprintf(file, "%d", symbol_table[intermediate_code[i].operandValue].address);
        }
        else if (strcmp(intermediate_code[i].operandType, "L") == 0)
        {
            // Operand is a literal, print its address
            fprintf(file, "%d", literal_table[intermediate_code[i].operandValue].address);
        }
        else if (strcmp(intermediate_code[i].operandType, "C") == 0)
        {
            // Operand is a constant, print its value
            fprintf(file, "%d", intermediate_code[i].operandValue);
        }

        fprintf(file, "\n");
    }

    fclose(file);
}

int main()
{
    const char *input_filename = "input.txt"; // Input file generated by Pass-I
    const char *output_filename = "output_pass2.txt";                                                         // Output file for machine code

    read_pass1_output(input_filename);
    generate_machine_code(output_filename);

    printf("Pass-II complete. Machine code written to %s\n", output_filename);

    return 0;
}
