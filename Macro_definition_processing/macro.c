#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_MACROS 10
#define MAX_LINES 100
#define MAX_PARAMS 5
#define MAX_NAME_LENGTH 20
#define MAX_KEY_PARAMS 5
#define MAX_EV_PARAMS 5

// Data structures for MDT, MNT, PNTAB, KPDTAB, EVNTAB, SSNTAB, and SSTAB
typedef struct
{
    int index;
    char definition[MAX_LINES][MAX_NAME_LENGTH];
} MDT;

typedef struct
{
    char name[MAX_NAME_LENGTH];
    int mdt_index;
    int param_count;
    char params[MAX_PARAMS][MAX_NAME_LENGTH];
} MNT;

typedef struct
{
    char macro_name[MAX_NAME_LENGTH];
    char param_name[MAX_NAME_LENGTH];
} ParamTable;

typedef struct
{
    char param_name[MAX_NAME_LENGTH];
    char default_value[MAX_NAME_LENGTH];
} KPDTAB;

typedef struct
{
    char ev_name[MAX_NAME_LENGTH];
} EVNTAB;

typedef struct
{
    char ss_name[MAX_NAME_LENGTH];
} SSNTAB;

typedef struct
{
    char symbol[MAX_NAME_LENGTH];
    int value;
} SSTAB;

// Global arrays for tables
MDT mdt[MAX_MACROS];
MNT mnt[MAX_MACROS];
ParamTable pntab[MAX_PARAMS];
KPDTAB kpdt[MAX_KEY_PARAMS];
EVNTAB evntab[MAX_EV_PARAMS];
SSNTAB ssntab[MAX_EV_PARAMS];
SSTAB sstab[MAX_EV_PARAMS];

int mnt_count = 0;
int mdt_count = 0;
int pntab_count = 0;
int kpdt_count = 0;
int evntab_count = 0;
int ssntab_count = 0;
int sstab_count = 0;

// Error detection for duplicate macros
int findMacroInMNT(char *name)
{
    for (int i = 0; i < mnt_count; i++)
    {
        if (strcmp(mnt[i].name, name) == 0)
        {
            return i;
        }
    }
    return -1;
}

void processMacro(char *name, char params[MAX_PARAMS][MAX_NAME_LENGTH], int paramCount, FILE *inputFile, FILE *outputFile)
{
    if (findMacroInMNT(name) != -1)
    {
        fprintf(outputFile, "Error: Duplicate macro name '%s'\n", name);
        return;
    }

    // Add to MNT
    strcpy(mnt[mnt_count].name, name);
    mnt[mnt_count].mdt_index = mdt_count;
    mnt[mnt_count].param_count = paramCount;

    // Store parameters in PNTAB and KPDTAB (key parameters for simplicity)
    for (int i = 0; i < paramCount; i++)
    {
        strcpy(pntab[pntab_count].macro_name, name);
        strcpy(pntab[pntab_count].param_name, params[i]);
        pntab_count++;

        // For simplicity, treat every second parameter as a key parameter
        if (i % 2 == 0)
        {
            strcpy(kpdt[kpdt_count].param_name, params[i]);
            strcpy(kpdt[kpdt_count].default_value, "DEFAULT"); // Assign default value
            kpdt_count++;
        }
    }

    // Add to EVNTAB and SSNTAB
    strcpy(evntab[evntab_count++].ev_name, "EV_PARAM");
    strcpy(ssntab[ssntab_count++].ss_name, "SS_PARAM");

    // Add symbols to SSTAB (simple hardcoded example)
    strcpy(sstab[sstab_count].symbol, "SYMBOL_1");
    sstab[sstab_count++].value = 100;

    mnt_count++;

    // Read macro definition lines
    char line[MAX_NAME_LENGTH];
    while (fgets(line, sizeof(line), inputFile))
    {
        if (strncmp(line, "MEND", 4) == 0)
        {
            strcpy(mdt[mdt_count].definition[mdt[mdt_count].index], line);
            mdt[mdt_count].index++;
            break;
        }
        strcpy(mdt[mdt_count].definition[mdt[mdt_count].index], line);
        mdt[mdt_count].index++;
    }

    mdt_count++;
}

void displayTables(FILE *outputFile)
{
    fprintf(outputFile, "Macro Name Table (MNT):\n");
    for (int i = 0; i < mnt_count; i++)
    {
        fprintf(outputFile, "Name: %s, MDT Index: %d, Params: ", mnt[i].name, mnt[i].mdt_index);
        for (int j = 0; j < mnt[i].param_count; j++)
        {
            fprintf(outputFile, "%s ", mnt[i].params[j]);
        }
        fprintf(outputFile, "\n");
    }

    fprintf(outputFile, "\nMacro Definition Table (MDT):\n");
    for (int i = 0; i < mdt_count; i++)
    {
        fprintf(outputFile, "MDT Entry %d:\n", i);
        for (int j = 0; j < mdt[i].index; j++)
        {
            fprintf(outputFile, "%s", mdt[i].definition[j]);
        }
    }

    fprintf(outputFile, "\nParameter Name Table (PNTAB):\n");
    for (int i = 0; i < pntab_count; i++)
    {
        fprintf(outputFile, "Macro: %s, Parameter: %s\n", pntab[i].macro_name, pntab[i].param_name);
    }

    fprintf(outputFile, "\nKeyword Parameter Default Table (KPDTAB):\n");
    for (int i = 0; i < kpdt_count; i++)
    {
        fprintf(outputFile, "Parameter: %s, Default Value: %s\n", kpdt[i].param_name, kpdt[i].default_value);
    }

    fprintf(outputFile, "\nEV Name Table (EVNTAB):\n");
    for (int i = 0; i < evntab_count; i++)
    {
        fprintf(outputFile, "EV Name: %s\n", evntab[i].ev_name);
    }

    fprintf(outputFile, "\nSS Name Table (SSNTAB):\n");
    for (int i = 0; i < ssntab_count; i++)
    {
        fprintf(outputFile, "SS Name: %s\n", ssntab[i].ss_name);
    }

    fprintf(outputFile, "\nSymbol Table (SSTAB):\n");
    for (int i = 0; i < sstab_count; i++)
    {
        fprintf(outputFile, "Symbol: %s, Value: %d\n", sstab[i].symbol, sstab[i].value);
    }
}

int main()
{
    FILE *inputFile = fopen("input.txt", "r");
    FILE *outputFile = fopen("output.txt", "w");

    if (!inputFile)
    {
        printf("Error opening input file.\n");
        return 1;
    }
    if (!outputFile)
    {
        printf("Error opening output file.\n");
        return 1;
    }

    char line[MAX_NAME_LENGTH];
    while (fgets(line, sizeof(line), inputFile))
    {
        if (strncmp(line, "MACRO", 5) == 0)
        {
            // Process macro definition
            fgets(line, sizeof(line), inputFile);
            char macro_name[MAX_NAME_LENGTH];
            char params[MAX_PARAMS][MAX_NAME_LENGTH];
            int paramCount = 0;

            // Extract macro name and parameters
            char *token = strtok(line, " \n");
            strcpy(macro_name, token);
            while ((token = strtok(NULL, " ,\n")) != NULL)
            {
                strcpy(params[paramCount++], token);
            }

            processMacro(macro_name, params, paramCount, inputFile, outputFile);
        }
    }

    displayTables(outputFile);

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}
