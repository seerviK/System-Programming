mnt_list = []
pntab_list = []
kpdtab_list = []
evntab_list = []
sstab_list = []
ssntab_list = []
mdt = []

def process_macro_file(file_name):
    try:
        with open(file_name, 'r') as file:
            lines = file.readlines()
            is_macro = False
            macro_name = ""
            current_pntab = []
            current_evntab = []
            current_ssntab = []
            pos_params = 0
            key_params = 0
            mdt_ptr = len(mdt)
            kpdtab_index = len(kpdtab_list)
            sstab_index = len(sstab_list)
            has_keywords = False
            counter = 0

            for line in lines:
                line = line.strip()
                if line.startswith("MACRO"):
                    is_macro = True
                    current_evntab = []
                    current_ssntab = []
                    continue
                elif line.startswith("MEND"):
                    is_macro = False
                    counter -= 1
                    mnt = {
                        'name': macro_name,
                        'pos_params': pos_params,
                        'key_params': key_params,
                        'expansion_vars': len(current_evntab),
                        'mdt_ptr': mdt_ptr,
                        'kpdtab_ptr': kpdtab_index if has_keywords else -1,
                        'sst_ptr': sstab_index
                    }
                    mnt_list.append(mnt)
                    mdt_ptr = len(mdt)
                    pntab_list.append(current_pntab)
                    evntab_list.append(current_evntab)
                    ssntab_list.append(current_ssntab)
                    current_pntab = []
                    macro_name = ""
                    kpdtab_index = len(kpdtab_list)
                    sstab_index = len(sstab_list)
                    pos_params = 0
                    key_params = 0
                    continue

                if is_macro:
                    if not macro_name:
                        counter -= 1
                        parts = line.split()
                        macro_name = parts[0]
                        for par in parts[1:]:
                            if par.startswith("."):
                                sstab_list.append((par, counter))
                                current_pntab.append(par)
                                pos_params += 1
                            else:
                                parax = par.split(",")
                                for param in parax:
                                    if "=" in param:
                                        key_value = param.split("=")
                                        kpdtab_list.append((key_value[0], key_value[1] if len(key_value) > 1 else None))
                                        current_pntab.append(key_value[0])
                                        pos_params += 1
                                        key_params += 1
                                        has_keywords = True
                                    else:
                                        current_pntab.append(param)
                                        pos_params += 1
                    elif line.startswith("LCL"):
                        vars = line[3:].strip().split(",")
                        for var in vars:
                            current_evntab.append(var.strip())
                            ev_index = current_evntab.index(var) + 1
                            mdt.append(line.replace(var, f"(E,{ev_index})"))
                    else:
                        tokens = line.split()
                        replaced_instruction = ""
                        symbol_name = ""
                        for token in tokens:
                            if token.startswith("."):
                                symbol_name = token.lstrip(".").strip()
                                current_ssntab.append(symbol_name)
                                sstab_list.append((symbol_name, counter))
                            elif "," in token:
                                sub_tokens = token.split(",")
                                for sub_token in sub_tokens:
                                    index = current_pntab.index(sub_token) if sub_token in current_pntab else -1
                                    if index != -1:
                                        replaced_instruction += f" (P,{index + 1})"
                                    elif sub_token in current_evntab:
                                        ev_index = current_evntab.index(sub_token) + 1
                                        replaced_instruction += f" (E,{ev_index})"
                                    else:
                                        replaced_instruction += sub_token
                                replaced_instruction += " "
                            else:
                                index = current_pntab.index(token) if token in current_pntab else -1
                                if index != -1:
                                    replaced_instruction += f" (P,{index + 1})"
                                elif token in current_evntab:
                                    ev_index = current_evntab.index(token) + 1
                                    replaced_instruction += f" (E,{ev_index})"
                                else:
                                    replaced_instruction += token
                        mdt.append(replaced_instruction.replace(f".{symbol_name}", "").strip())

                counter += 1
    except IOError as e:
        print(f"Error reading macro file: {e}")


def print_tables(file_name):
    try:
        with open(file_name, 'w') as file:
            file.write("MNT:\n")
            for mnt in mnt_list:
                file.write(f" Name={mnt['name']}, PosParams={mnt['pos_params']}, "
                           f"KeyParams={mnt['key_params']}, ExpVars={mnt['expansion_vars']}, "
                           f"MDT Ptr={mnt['mdt_ptr']}, KPDTAB Ptr={mnt['kpdtab_ptr']}, SST Ptr={mnt['sst_ptr']}\n")

            file.write("\nPNTAB:\n")
            for pntab in pntab_list:
                for i, pn in enumerate(pntab):
                    pntab[i] = pn.replace("&", "")
                file.write(f" {pntab}\n")

            file.write("\nKPDTAB:\n")
            for kpdtab in kpdtab_list:
                file.write(f" {kpdtab[0].replace('&', '')} = {kpdtab[1]}\n")

            file.write("\nMDT:\n")
            c = 0
            for line in mdt:
                for mnt in mnt_list:
                    if mnt['mdt_ptr'] == c:
                        file.write("\n")
                file.write(line + "\n")
                c += 1

            file.write("\nSSTAB:\n")
            for sstab in sstab_list:
                file.write(f"{sstab[0]}: {sstab[1]}\n")

            file.write("\nSSNTAB:\n")
            for ssntab in ssntab_list:
                file.write(f" {ssntab}\n")

            file.write("\nEVNTAB:\n")
            for evntab in evntab_list:
                file.write(f" {evntab}\n")
    except IOError as e:
        print(f"Error writing to file: {e}")


# Example usage
process_macro_file("input.txt")
print_tables("output.txt")