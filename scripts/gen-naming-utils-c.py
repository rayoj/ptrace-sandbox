#!/usr/bin/python3

import sys
def main():
    print("// This file is generated by automatic")
    print("// script, don't modify it, modify the generator instead.")
    print("// Generator can be found under scripts/")
    print("#include \"naming_utils.h\"")
    print("#include <sys/syscall.h>")
    print("")
    print("/* returns string description of syscall, or NULL */")
    print("const char* get_syscall_name(long id) {")
    table_sz = 1000
    print("    static int initialized = 0;")
    print("    static const char* names[" + str(table_sz) + "] = {0};")
    print("    if (!initialized) {")
    print("        initialized = 1;")
    for syscall in sys.stdin:
        syscall = syscall.strip()
        friendly_name = ""
        if (syscall.startswith("SYS_")):
            friendly_name = syscall[4:]
        else:
            friendly_name = syscall
        print("#ifdef " + syscall)
        print("        if (0 <= " + syscall + " && " + syscall + " < " + str(table_sz) + ")")
        
        print("            names[" + syscall + "] = \"" + friendly_name + "\";")
        print("#endif")
    print("    }")
    print("    if (0 <= id && id < " + str(table_sz) + ")")
    print("        return names[id];")
    print("    return 0;")
    print("}")
    
main()