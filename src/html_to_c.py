#!/bin/python

# creates a C header with html saved as a string from the given html file.

import os
import sys

def append_html(lst: list, char):
    if char == "\"":
        lst.append("\\\"")
    else: 
        lst.append(char)

def convert_line(line: str) -> str:

    skip_char = [" ", "\n", "\t"]
    depth = 0

    out = ["\""]
    begin = []

    for char in line:
        if char not in skip_char and depth == 0:
            depth = 1
        if depth == 0:
            begin.append(char)
        if depth == 1 and char != '\n':
            append_html(out, char)

    out.append("\"")
    return "".join(begin + out)

def main():
    try:
        path = sys.argv[1]
    except:
        print("no file given, use:\n\n\t ./{} FILE\n".format(sys.argv[0]))
        return

    if path[-5:] != '.html':
        print("{} is not an .html".format(path))
        return

    out_path = path[:-5] + ".h"

    with open(path,'r') as file, open(out_path, 'w') as save_file:

        name = os.path.basename(out_path).replace('.','_')
        save_file.write("#ifndef " + name.upper() + '\n')
        save_file.write("#define " + name.upper() + "\n\n")

        save_file.write("char " + name.lower() + "[] = {\n")

        for line in file.readlines():
            save_file.write(convert_line(line) + '\n')

        save_file.write("};\n")
        save_file.write("#endif\n")


if __name__ == '__main__':
    main()
