mnt_list = []
pntab_list = []
kpdtab_list = []
mdt = []
sstab_list = []
ssntab_list = []
evntab_list = []
aptab_list = []

def read_tables(file_name):
    global mnt_list, pntab_list, kpdtab_list, mdt, sstab_list, ssntab_list, evntab_list
    try:
        with open(file_name, 'r') as file:
            current_section = ''
            for line in file:
                line = line.strip()
                if line.startswith("MNT:"):
                    current_section = "MNT"
                elif line.startswith("PNTAB:"):
                    current_section = "PNTAB"
                elif line.startswith("KPDTAB:"):
                    current_section = "KPDTAB"
                elif line.startswith("MDT:"):
                    current_section = "MDT"
                elif line.startswith("SSTAB:"):
                    current_section = "SSTAB"
                elif line.startswith("SSNTAB:"):
                    current_section = "SSNTAB"
                elif line.startswith("EVNTAB:"):
                    current_section = "EVNTAB"
                elif line:
                    if current_section == "MNT":
                        parts = line.split(",")
                        name = parts[0].split("=")[1].strip()
                        pos_params = int(parts[1].split("=")[1].strip())
                        key_params = int(parts[2].split("=")[1].strip())
                        mdt_ptr = int(parts[4].split("=")[1].strip())
                        kpdt_ptr = int(parts[5].split("=")[1].strip())
                        sst_ptr = int(parts[6].split("=")[1].strip())
                        mnt_list.append({"name": name, "pos_params": pos_params, "key_params": key_params, 
                                         "mdt_ptr": mdt_ptr, "kpdt_ptr": kpdt_ptr, "sst_ptr": sst_ptr})
                    elif current_section == "PNTAB":
                        pntab = line[2:-1].split(", ")
                        pntab_list.append(pntab)
                    elif current_section == "KPDTAB":
                        parts = line.split("=")
                        kpdtab_list.append({"param_name": parts[0].strip(), "default_value": parts[1].strip() if len(parts) > 1 else "null"})
                    elif current_section == "MDT":
                        mdt.append(line.strip())
                    elif current_section == "SSTAB":
                        parts = line.split(":")
                        sstab_list.append({"symbol_name": parts[0].strip(), "location_in_mdt": int(parts[1].strip())})
                    elif current_section == "SSNTAB":
                        ssntab_list.append(line[1:-1].split(", "))
                    elif current_section == "EVNTAB":
                        evntab_list.append(line[1:-1].split(", "))
    except Exception as e:
        print(f"Error reading tables file: {e}")

def process_source_file(input_file, output_file):
    global aptab_list
    try:
        with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
            in_macro_expansion = False
            line_number = 0

            for line in infile:
                line = line.strip()
                line_number += 1

                if line == "START":
                    outfile.write(line + '\n')
                    in_macro_expansion = True
                    line_number -= 1
                elif line == "END":
                    outfile.write(line + '\n')
                    in_macro_expansion = False
                    line_number -= 1
                elif in_macro_expansion:
                    parts = line.split(" ")
                    macro_name = parts[0]
                    arguments = parts[1].split(",")
                    print(arguments)

                    mnt = next((m for m in mnt_list if m["name"] == macro_name), None)
                    if mnt:
                        arguments = arguments[:len(arguments)]
                        aptab_list.append({"macro_name": macro_name, "arguments": arguments, "line_number": line_number})

                        arg_map = {}
                        pntab = pntab_list[mnt_list.index(mnt)]

                        for i in range(min(mnt["pos_params"], len(pntab))):
                            if i < len(arguments):
                                if "=" in arguments[i]:
                                    arg_map[pntab[i]] = arguments[i].split("=")[1]
                                else:
                                    arg_map[pntab[i]] = arguments[i].strip() if arguments[i].strip() else "null"

                        for i in range(mnt["kpdt_ptr"], min(mnt["kpdt_ptr"] + mnt["key_params"], len(kpdtab_list))):
                            kpdtab = kpdtab_list[i]
                            if kpdtab["param_name"] in arg_map and arg_map[kpdtab["param_name"]] == "null":
                                arg_map[kpdtab["param_name"]] = kpdtab["default_value"]

                        mdt_end_ptr = len(mdt)
                        if mnt_list.index(mnt) < len(mnt_list) - 1:
                            mdt_end_ptr = mnt_list[mnt_list.index(mnt) + 1]["mdt_ptr"]

                        for i in range(mnt["mdt_ptr"], mdt_end_ptr):
                            instruction = mdt[i]
                            instruction_parts = instruction.split(" ")
                            instruction = "".join([f"{part.strip()} " for part in instruction_parts])

                            if "MEND" in instruction:
                                break

                            for param, value in arg_map.items():
                                param_index = pntab.index(param) if param in pntab else -1
                                if param_index != -1:
                                    instruction = instruction.replace(f"(P,{param_index + 1})", value)

                            outfile.write(instruction + '\n')
                    else:
                        outfile.write(line + '\n')
                else:
                    outfile.write(line + '\n')
    except Exception as e:
        print(f"Error processing source file: {e}")

def save_aptab_to_file(aptab_file_name):
    try:
        with open(aptab_file_name, 'w') as file:
            for aptab in aptab_list:
                file.write(f"APTAB(macro_name='{aptab['macro_name']}', arguments={aptab['arguments']}, line_number={aptab['line_number']})\n")
        print(f"APTAB saved to file: {aptab_file_name}")
    except Exception as e:
        print(f"Error writing APTAB to file: {e}")

mnt_list, pntab_list, kpdtab_list, mdt, sstab_list, ssntab_list, evntab_list = [], [], [], [], [], [], []
read_tables("input_tables.txt")
process_source_file("input.txt", "expanded.txt")
save_aptab_to_file("APTAB_output.txt")
print("Macro expansion completed. Check 'expanded.txt' for the result.")