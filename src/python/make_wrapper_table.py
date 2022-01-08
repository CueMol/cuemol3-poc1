#!/usr/bin/env python

import argparse

from pathlib import Path


def arguemtn_parser():
    parser = argparse.ArgumentParser(description="make wrapper table js file")
    parser.add_argument("--output_js",type=str,required=True)
    parser.add_argument("--input_dir",type=str,default="results")
    args = parser.parse_args()
    return args


def main():
    args = arguemtn_parser()
    input_dir = Path(args.input_dir).resolve()
    with open(args.output_js, "w") as f:
        f.write("// autogenerated by make_wrapper_table.py\n")
        f.write("\n")
        print(f"input_dir: {input_dir}")
        file_list = sorted(list(input_dir.glob("*.js")))

        for in_js in file_list:
            stem = in_js.stem
            f.write(f"import {{ {stem} }} from './wrappers/{stem}.js'\n")

        f.write("\n")
        f.write("\n")
        f.write("const wrapper_map = {\n")

        for in_js in file_list:
            stem = in_js.stem
            f.write(f"    \'{stem}\': {stem},\n")

        f.write("};\n")
        f.write("\n")
        f.write("export { wrapper_map };\n")
        f.write("\n")

    print(f"output {args.output_js} ok")

if __name__ == "__main__":
    main()
