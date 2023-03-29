#!/bin/python

# creates a C header with data the provided file.

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

def start_header(file, out_path: str, name: str):
    file.write("#ifndef " + name.upper() + '\n')
    file.write("#define " + name.upper() + "\n\n")


def end_header(file):
    file.write("#endif\n")

def save_html_header(path: str, out_path: str):
    with open(path,'r') as file, open(out_path, 'w') as save_file:

        name = os.path.basename(out_path).replace('.','_')
        start_header(save_file, out_path, name)

        save_file.write("char " + name.lower() + "[] = {\n")
        for line in file.readlines():
            save_file.write(convert_line(line) + '\n')
        
        save_file.write("};\n")

        end_header(save_file)

def save_binary_header(path: str, out_path: str):
    with open(path,'rb') as file, open(out_path, 'w') as save_file:

        name = os.path.basename(out_path).replace('.','_')
        start_header(save_file, out_path, name)

        save_file.write("#include \"http.h\"\n\n")

        save_file.write("char " + name + "_data[] = {\n")

        i = 0
        for byte in file.read():
            save_file.write(str(byte) + ',')
            if i == 20:
                save_file.write('\n')
                i = 0
            i += 1

        save_file.write("};\n\n")

        save_file.write("raw_http_get_data " + name + " = {\n")
        save_file.write("    .type = \"image/jpeg\",\n")
        save_file.write("    .data = (void*)" + name + "_data,\n")
        save_file.write("    .size = sizeof(" + name + "_data)\n")
        save_file.write("};\n\n")

        end_header(save_file)


def main():
    try:
        path = sys.argv[1]
    except:
        print("no file given, use:\n\n\t ./{} FILE\n".format(sys.argv[0]))
        return

    file_name, extention = os.path.splitext(path)
    out_path = file_name + ".h"

    if extention == '.html':
        print("{} is in .html fromat".format(path))
        save_html_header(path, out_path)
    elif extention == '.jpg':
        print("{} is in .jpg fromat".format(path))
        save_binary_header(path, out_path)

if __name__ == '__main__':
    main()
