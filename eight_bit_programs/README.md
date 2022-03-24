# Sample Programs
Here are a few sample programs on the 8-bit computer, written in its ISA assembly. The parsing, etc., are done using a program called `customasm`. The assembly-machine-code correspondance is found in `rules`.

File `generate.sh` takes an `asm` file in this directory and assembles it using `customasm` into the content of a C-array. An Arduino program is the compiled with this C-array. Arduino will then manipulate the 8-bit computer's RAM to input in the instructions.