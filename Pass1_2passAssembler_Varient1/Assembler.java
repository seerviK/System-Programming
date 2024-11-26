import java.io.*;
import java.util.*;

public class Assembler {
    static class MOTEntry {
        String name, type;
        int opcode;

        MOTEntry(String name, String type, int opcode) {
            this.name = name;
            this.type = type;
            this.opcode = opcode;
        }
    }

    static class Register {
        String name;
        int code;

        Register(String name, int code) {
            this.name = name;
            this.code = code;
        }
    }

    static class Symbol {
        String name;
        int address;

        Symbol(String name, int address) {
            this.name = name;
            this.address = address;
        }
    }

    static class Literal {
        String name;
        int address;
        int poolNo;

        Literal(String name, int address, int poolNo) {
            this.name = name;
            this.address = address;
            this.poolNo = poolNo;
        }
    }

    static class Pool {
        int startIndex;
        int literalCount;

        Pool(int startIndex, int literalCount) {
            this.startIndex = startIndex;
            this.literalCount = literalCount;
        }
    }

    static class IntermediateCode {
        String operation;
        int code;
        int reg;
        String operandType;
        int operandValue;
        int lc;

        IntermediateCode(String operation, int code, int reg, String operandType, int operandValue, int lc) {
            this.operation = operation;
            this.code = code;
            this.reg = reg;
            this.operandType = operandType;
            this.operandValue = operandValue;
            this.lc = lc;
        }
    }

    static MOTEntry[] mot = {
            new MOTEntry("STOP", "IS", 0),
            new MOTEntry("ADD", "IS", 1),
            new MOTEntry("SUB", "IS", 2),
            new MOTEntry("MUL", "IS", 3),
            new MOTEntry("MOVER", "IS", 4),
            new MOTEntry("MOV", "IS", 5),
            new MOTEntry("COMP", "IS", 6),
            new MOTEntry("BC", "IS", 7),
            new MOTEntry("DIV", "IS", 8),
            new MOTEntry("READ", "IS", 9),
            new MOTEntry("PRINT", "IS", 10),
            new MOTEntry("START", "AD", 1),
            new MOTEntry("END", "AD", 2),
            new MOTEntry("ORIGIN", "AD", 3),
            new MOTEntry("EQU", "AD", 4),
            new MOTEntry("LTORG", "AD", 5),
            new MOTEntry("DS", "DL", 1),
            new MOTEntry("DC", "DL", 2)
    };

    static Register[] registers = {
            new Register("AREG", 1),
            new Register("BREG", 2),
            new Register("CREG", 3),
            new Register("DREG", 4)
    };

    static List<Symbol> symbolTable = new ArrayList<>();
    static List<Literal> literalTable = new ArrayList<>();
    static List<Pool> poolTable = new ArrayList<>();
    static List<IntermediateCode> intermediateCode = new ArrayList<>();

    static int locationCounter = 0;

    public static void processLine(String line) {
        String label = null, opcode = null, operand1 = null, operand2 = null;
        StringTokenizer st = new StringTokenizer(line, " ,\t");

        if (st.hasMoreTokens())
            label = st.nextToken();
        if (label != null && label.endsWith(":")) {
            label = label.substring(0, label.length() - 1);
            if (st.hasMoreTokens())
                opcode = st.nextToken();
        } else {
            opcode = label;
            label = null;
        }

        if (st.hasMoreTokens())
            operand1 = st.nextToken();
        if (st.hasMoreTokens())
            operand2 = st.nextToken();

        if (label != null) {
            symbolTable.add(new Symbol(label, locationCounter));
        }

        for (MOTEntry entry : mot) {
            if (entry.name.equals(opcode)) {
                switch (entry.type) {
                    case "AD":
                        if ("START".equals(opcode)) {
                            locationCounter = Integer.parseInt(operand1);
                            intermediateCode.add(new IntermediateCode("AD", entry.opcode, -1, "C", locationCounter,
                                    locationCounter));
                        } else if ("END".equals(opcode)) {
                            fillLiteralAddresses();
                            intermediateCode
                                    .add(new IntermediateCode("AD", entry.opcode, -1, null, -1, locationCounter));
                        } else if ("LTORG".equals(opcode)) {
                            fillLiteralAddresses();
                            poolTable.add(new Pool(literalTable.size(), 0));
                        }
                        break;
                    case "IS":
                        int regCode = -1;
                        if (operand1 != null) {
                            for (Register reg : registers) {
                                if (reg.name.equals(operand1)) {
                                    regCode = reg.code;
                                    break;
                                }
                            }
                        }
                        if (operand2 != null && operand2.startsWith("=")) {
                            literalTable.add(new Literal(operand2, -1, poolTable.size() - 1));
                            poolTable.get(poolTable.size() - 1).literalCount++;
                            intermediateCode.add(new IntermediateCode("IS", entry.opcode, regCode, "L",
                                    literalTable.size() - 1, locationCounter));
                        } else if (operand2 != null) {
                            int symIndex = -1;
                            for (int j = 0; j < symbolTable.size(); j++) {
                                if (symbolTable.get(j).name.equals(operand2)) {
                                    symIndex = j;
                                    break;
                                }
                            }
                            if (symIndex == -1) {
                                symbolTable.add(new Symbol(operand2, locationCounter));
                                symIndex = symbolTable.size() - 1;
                            }
                            intermediateCode.add(
                                    new IntermediateCode("IS", entry.opcode, regCode, "S", symIndex, locationCounter));
                        }
                        locationCounter++;
                        break;
                    case "DL":
                        if ("DS".equals(opcode)) {
                            int size = Integer.parseInt(operand1);
                            symbolTable.get(symbolTable.size() - 1).address = locationCounter;
                            intermediateCode
                                    .add(new IntermediateCode("DL", entry.opcode, -1, "C", size, locationCounter));
                            locationCounter += size;
                        } else if ("DC".equals(opcode)) {
                            int value = Integer.parseInt(operand1);
                            symbolTable.get(symbolTable.size() - 1).address = locationCounter;
                            intermediateCode
                                    .add(new IntermediateCode("DL", entry.opcode, -1, "C", value, locationCounter));
                            locationCounter++;
                        }
                        break;
                }
                return;
            }
        }
        System.out.println("Error: Invalid instruction " + opcode);
    }

    public static void readFileAndProcessLines(String filename) {
        try (BufferedReader br = new BufferedReader(new FileReader(filename))) {
            String line;
            while ((line = br.readLine()) != null) {
                processLine(line);
            }
        } catch (IOException e) {
            System.out.println("Error: Unable to open file " + filename);
        }
    }

    public static void fillLiteralAddresses() {
        for (int i = poolTable.get(poolTable.size() - 1).startIndex; i < literalTable.size(); i++) {
            if (literalTable.get(i).address == -1) {
                literalTable.get(i).address = locationCounter++;
            }
        }
    }

    public static void writeOutputToFile(String filename) {
        try (PrintWriter pw = new PrintWriter(new FileWriter(filename))) {
            pw.println("Symbol Table:");
            for (int i = 0; i < symbolTable.size(); i++) {
                Symbol s = symbolTable.get(i);
                pw.printf("%d    %s    %d%n", i, s.name, s.address);
            }

            pw.println("\nLiteral Table:");
            for (int i = 0; i < literalTable.size(); i++) {
                Literal l = literalTable.get(i);
                pw.printf("%d    %s    %d%n", i, l.name, l.address);
            }

            pw.println("\nPool Table:");
            for (int i = 0; i < poolTable.size(); i++) {
                Pool p = poolTable.get(i);
                pw.printf("%d    %d%n", i, p.startIndex);
            }

            pw.println("\nIntermediate Code with LC (Variant I form):");
            for (IntermediateCode ic : intermediateCode) {
                pw.printf("%d   (%s, %d)   ", ic.lc, ic.operation, ic.code);
                if (ic.reg != -1) {
                    pw.printf("(%d)   ", ic.reg);
                } else {
                    pw.print("      ");
                }
                if (ic.operandType != null) {
                    pw.printf("(%s, %d)", ic.operandType, ic.operandValue);
                }
                pw.println();
            }
        } catch (IOException e) {
            System.out.println("Error: Unable to write to file " + filename);
        }
    }

    public static void main(String[] args) {
        String inputFile = "input.txt"; // Specify the input file path here
        String outputFile = "output.txt"; // Specify the output file path here

        System.out.println("Output generated in output.txt");
        poolTable.add(new Pool(0, 0)); // Initialize first pool

        readFileAndProcessLines(inputFile);
        writeOutputToFile(outputFile);
    }
}
