# Digital Signal Generator

A C++ application for generating and visualizing digital signals using various line coding schemes, scrambling techniques, and modulation methods with OpenGL graphics.


## Features

- **Line Coding**: NRZ-L, NRZ-I, Manchester, Differential Manchester, AMI
- **Scrambling**: B8ZS, HDB3
- **Modulation**: PCM, Delta Modulation
- **Real-time Visualization**: Interactive OpenGL waveform display

## Language & Libraries

- **Language**: C++
- **Graphics**: OpenGL, FreeGLUT
- **Standard Libraries**: iostream, cstring, cmath

## Prerequisites

- MinGW/GCC compiler (C++11 or later)
- FreeGLUT library
- OpenGL-compatible graphics card

## Installation

1. Download FreeGLUT library files (already included in `GL/` folder)
2. Ensure `freeglut.dll` and `libfreeglut.a` are in the project root directory

## Compilation

g++ signal_generator.cpp -o signal_generator.exe -L. -I. -lfreeglut -lopengl32 -lglu32


text

**Note**: The `-L.` flag points to the current directory where `libfreeglut.a` is located.

## Execution

./signal_generator.exe

text

Or simply double-click `signal_generator.exe` in Windows Explorer.

## Usage Examples

### Example 1: NRZ-I Encoding

**Input:**
=== Digital Signal Generator ===

Digital Input

Analog Input (PCM/DM)
Choice: 1

Enter binary data: 10010101

Select encoding:

NRZ-L

NRZ-I

Manchester

Diff Manchester

AMI
Choice: 1

text

**Output:**
Encoded Signal: 1 -1 1 -1
Longest Palindrome: 01010


OpenGL window opened. Close to exit...

text

![NRZ-L Output](Output-Images/nrz-l.png)

---


### Example 2: Differential Manchester

**Input:**
Enter binary data: 10101010

Select encoding:
Choice: 4

text

**Output:**
Encoded Signal: (Differential Manchester waveform)

text

![Differential Manchester Output](Output-Images/differential.png)

---


## Project Structure

dc-assignment/
├── GL/ # OpenGL/FreeGLUT headers folder
├── Output-Images/ # Output screenshots
│ ├── differential.png
│ ├── nrz-l.png
│ 
├── signal_generator.cpp # Main source code
├── signal_generator.exe # Compiled executable
├── freeglut.dll # FreeGLUT dynamic library
├── libfreeglut.a # FreeGLUT static library
└── README.md # This file

text

## Troubleshooting

**Issue**: `freeglut.dll not found`
- **Solution**: Ensure `freeglut.dll` is in the same directory as `signal_generator.exe`

**Issue**: Linker error `cannot find -lfreeglut`
- **Solution**: Use `-L.` flag to point to current directory:
g++ signal_generator.cpp -o signal_generator.exe -L. -I. -lfreeglut -lopengl32 -lglu32


text

**Issue**: OpenGL window doesn't appear
- **Solution**: Update graphics drivers and ensure OpenGL support is enabled

**Issue**: Compilation error with GL headers
- **Solution**: Verify `GL/` folder contains proper FreeGLUT headers

## License

Open-source project for educational purposes.