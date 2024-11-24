import os

class MOTEntry:
    def __init__(self, name, type, opcode):
        self.name = name
        self.type = type
        self.opcode = opcode

class Register:
    def __init__(self, name, code):
        self.name = name
        self.code = code

class Symbol:
    def __init__(self, name, address):
        self.name = name
        self.address = address

class Literal:
    def __init__(self, name, address, pool_no):
        self.name = name
        self.address = address
        self.pool_no = pool_no

class Pool:
    def __init__(self, start_index, literal_count):
        self.start_index = start_index
        self.literal_count = literal_count

class IntermediateCode:
    def __init__(self, operation, code, reg, operand_type, operand_value, lc):
        self.operation = operation
        self.code = code
        self.reg = reg
        self.operand_type = operand_type
        self.operand_value = operand_value
        self.lc = lc

mot = [
    MOTEntry("STOP", "IS", 0),
    MOTEntry("ADD", "IS", 1),
    MOTEntry("SUB", "IS", 2),
    MOTEntry("MUL", "IS", 3),
    MOTEntry("MOVER", "IS", 4),
    MOTEntry("MOV", "IS", 5),
    MOTEntry("COMP", "IS", 6),
    MOTEntry("BC", "IS", 7),
    MOTEntry("DIV", "IS", 8),
    MOTEntry("READ", "IS", 9),
    MOTEntry("PRINT", "IS", 10),
    MOTEntry("START", "AD", 1),
    MOTEntry("END", "AD", 2),
    MOTEntry("ORIGIN", "AD", 3),
    MOTEntry("EQU", "AD", 4),
    MOTEntry("LTORG", "AD", 5),
    MOTEntry("DS", "DL", 1),
    MOTEntry("DC", "DL", 2)
]

registers = [
    Register("AREG", 1),
    Register("BREG", 2),
    Register("CREG", 3),
    Register("DREG", 4)
]

symbol_table = []
literal_table = []
pool_table = []
intermediate_code = []

location_counter = 0

def process_line(line):
    global location_counter
    label = None
    opcode = None
    operand1 = None
    operand2 = None
    tokens = line.replace(',', ' ').split()

    if tokens:
        label = tokens.pop(0)
    if label and label.endswith(':'):
        label = label[:-1]
        if tokens:
            opcode = tokens.pop(0)
    else:
        opcode = label
        label = None

    if tokens:
        operand1 = tokens.pop(0)
    if tokens:
        operand2 = tokens.pop(0)

    if label:
        symbol_table.append(Symbol(label, location_counter))

    for entry in mot:
        if entry.name == opcode:
            if entry.type == "AD":
                if opcode == "START":
                    location_counter = int(operand1)
                    intermediate_code.append(IntermediateCode("AD", entry.opcode, -1, "C", location_counter, location_counter))
                elif opcode == "END":
                    fill_literal_addresses()
                    intermediate_code.append(IntermediateCode("AD", entry.opcode, -1, None, -1, location_counter))
                elif opcode == "LTORG":
                    fill_literal_addresses()
                    pool_table.append(Pool(len(literal_table), 0))
            elif entry.type == "IS":
                reg_code = -1
                if operand1:
                    for reg in registers:
                        if reg.name == operand1:
                            reg_code = reg.code
                            break
                if operand2 and operand2.startswith("="):
                    literal_table.append(Literal(operand2, -1, len(pool_table) - 1))
                    pool_table[-1].literal_count += 1
                    intermediate_code.append(IntermediateCode("IS", entry.opcode, reg_code, "L", len(literal_table) - 1, location_counter))
                elif operand2:
                    sym_index = -1
                    for j, sym in enumerate(symbol_table):
                        if sym.name == operand2:
                            sym_index = j
                            break
                    if sym_index == -1:
                        symbol_table.append(Symbol(operand2, location_counter))
                        sym_index = len(symbol_table) - 1
                    intermediate_code.append(IntermediateCode("IS", entry.opcode, reg_code, "S", sym_index, location_counter))
                location_counter += 1
            elif entry.type == "DL":
                if opcode == "DS":
                    size = int(operand1)
                    symbol_table[-1].address = location_counter
                    intermediate_code.append(IntermediateCode("DL", entry.opcode, -1, "C", size, location_counter))
                    location_counter += size
                elif opcode == "DC":
                    value = int(operand1)
                    symbol_table[-1].address = location_counter
                    intermediate_code.append(IntermediateCode("DL", entry.opcode, -1, "C", value, location_counter))
                    location_counter += 1
            return
    print(f"Error: Invalid instruction {opcode}")

def read_file_and_process_lines(filename):
    try:
        with open(filename, 'r') as file:
            for line in file:
                process_line(line.strip())
    except IOError:
        print(f"Error: Unable to open file {filename}")

def fill_literal_addresses():
    global location_counter
    for i in range(pool_table[-1].start_index, len(literal_table)):
        if literal_table[i].address == -1:
            literal_table[i].address = location_counter
            location_counter += 1

def write_output_to_file(filename):
    try:
        with open(filename, 'w') as file:
            file.write("Symbol Table:\n")
            for i, s in enumerate(symbol_table):
                file.write(f"{i}    {s.name}    {s.address}\n")

            file.write("\nLiteral Table:\n")
            for i, l in enumerate(literal_table):
                file.write(f"{i}    {l.name}    {l.address}\n")

            file.write("\nPool Table:\n")
            for i, p in enumerate(pool_table):
                file.write(f"{i}    {p.start_index}\n")

            file.write("\nIntermediate Code with LC (Variant I form):\n")
            for ic in intermediate_code:
                file.write(f"{ic.lc}   ({ic.operation}, {ic.code})   ")
                if ic.reg != -1:
                    file.write(f"({ic.reg})   ")
                else:
                    file.write("      ")
                if ic.operand_type:
                    file.write(f"({ic.operand_type}, {ic.operand_value})")
                file.write("\n")
    except IOError:
        print(f"Error: Unable to write to file {filename}")

def main():
    input_file = "input.txt"  # Specify the input file path here
    output_file = "output.txt"  # Specify the output file path here

    print("Output generated in output.txt")
    pool_table.append(Pool(0, 0))  # Initialize first pool

    read_file_and_process_lines(input_file)
    write_output_to_file(output_file)

if __name__ == "__main__":
    main()