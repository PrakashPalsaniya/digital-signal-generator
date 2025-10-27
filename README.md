# Digital Signal Generator

## Overview

This project is a comprehensive C++ application for generating, encoding, scrambling, and visualizing digital signals. It utilizes OpenGL and GLUT for graphical representation of various digital communication techniques, including line coding schemes, scrambling methods, and modulation techniques. The program provides both analytical outputs and interactive visualizations to aid in understanding digital signal processing concepts.

## Features

### Line Coding Schemes
- **NRZ-L (Non-Return-to-Zero Level)**: Maps '1' to +1 and '0' to -1
- **NRZ-I (Non-Return-to-Zero Inverted)**: Inverts signal level on '1', stays same on '0'
- **Manchester Encoding**: Uses transitions to represent bits (rising for '0', falling for '1')
- **Differential Manchester**: Similar to Manchester but with differential encoding
- **AMI (Alternate Mark Inversion)**: Alternates polarity for '1's, '0's are zero

### Scrambling Techniques
- **B8ZS (Bipolar with 8 Zeros Substitution)**: Replaces 8 consecutive zeros with a specific pattern
- **HDB3 (High-Density Bipolar 3)**: Replaces 4 consecutive zeros with violation patterns

### Modulation Methods
- **PCM (Pulse Code Modulation)**: Quantizes analog signals to binary
- **DM (Delta Modulation)**: Encodes analog signals using delta encoding

### Analytical Functions
- **Longest Palindromic Substring**: Uses Manacher's algorithm to find the longest palindrome in input data
- **Longest Zero Sequence**: Identifies the longest consecutive sequence of zeros in the encoded signal

### Visualization
- Interactive OpenGL window displaying signal waveforms
- Grid lines, axis labels, and signal value annotations
- Support for different bit rates and encoding types

## Prerequisites

### Software Requirements
- **C++ Compiler**: GCC (g++) or compatible compiler supporting C++11 or later
- **OpenGL and GLUT**: FreeGLUT library for graphics rendering
- **Operating System**: Windows (tested), Linux (with appropriate GLUT installation)

### Hardware Requirements
- Graphics card supporting OpenGL
- Minimum 512MB RAM
- Display resolution of at least 1024x768

## Installation

### Step 1: Install GLUT
1. Download FreeGLUT from the official website: https://freeglut.sourceforge.net/
2. Extract the files to a directory (e.g., `C:\freeglut`)
3. Copy `freeglut.dll` to your project directory (`dc-assignment/`) or to `C:\Windows\System32\`

### Step 2: Verify Project Files
Ensure the following files are present in the `dc-assignment/` directory:
- `signal_generator.cpp` (main source code)
- `libfreeglut.a` (static library)
- `freeglut.dll` (dynamic library)
- `GL/` directory (if present, contains additional OpenGL headers)

## Build Instructions

### Using g++ (GCC)
1. Open Command Prompt or Terminal
2. Navigate to the project directory:
   ```
   cd dc-assignment
   ```
3. Compile the program:
   ```
   g++ signal_generator.cpp -o signal_generator.exe -lfreeglut -lglu32 -lopengl32 -Wl,--subsystem,windows
   ```
   - `-lfreeglut`: Links to FreeGLUT library
   - `-lglu32 -lopengl32`: Links to OpenGL and GLU libraries
   - `-Wl,--subsystem,windows`: Creates a Windows GUI application

### Using MinGW
If using MinGW, ensure the library paths are correctly set:
```
g++ signal_generator.cpp -o signal_generator.exe -L/path/to/freeglut/lib -lfreeglut -lglu32 -lopengl32
```

### Using Visual Studio
1. Create a new C++ Console Application project
2. Add `signal_generator.cpp` to the project
3. Configure project properties:
   - Include directories: Add path to FreeGLUT headers
   - Library directories: Add path to FreeGLUT libraries
   - Linker input: Add `freeglut.lib`, `opengl32.lib`, `glu32.lib`
4. Build the project

### Troubleshooting Build Issues
- **Missing GLUT headers**: Ensure FreeGLUT is properly installed and paths are set
- **Linker errors**: Verify that all required libraries are linked
- **OpenGL not found**: Install graphics drivers and ensure OpenGL support

## Run Instructions

### Basic Execution
1. After successful compilation, run the executable:
   ```
   ./signal_generator.exe
   ```
2. The program will prompt for input through the console

### Input Options
The program supports two main input types:

#### 1. Digital Input
- Enter binary data (e.g., `10101011`)
- Choose encoding scheme
- Optionally apply scrambling

#### 2. Analog Input
- Choose modulation type (PCM or DM)
- Enter number of samples
- Input analog values
- For PCM: Specify bits per sample

### Example Run
```
=== Digital Signal Generator ===
1. Digital Input
2. Analog Input (PCM/DM)
Choice: 1
Binary data: 101010
1. NRZ-L
2. NRZ-I
3. Manchester
4. Diff Manchester
5. AMI
Choice: 3

Signal: 1 -1 -1 1 1 -1 -1 1

OpenGL window opened. Close to exit...
```

## Usage Examples

### Example 1: NRZ-L Encoding
```
Input: 1010
Output: 1 -1 1 -1
Visualization: Alternating high and low levels
```

### Example 2: Manchester Encoding
```
Input: 10
Output: -1 1 1 -1
Visualization: Transitions at each bit boundary
```

### Example 3: AMI with B8ZS Scrambling
```
Input: 00000000 (8 zeros)
Output: 0 0 0 1 -1 0 1 -1 (after B8ZS)
Visualization: Shows violation pulses
```

### Example 4: PCM Modulation
```
Analog samples: 1.2 2.5 0.8
Bits: 2
Output: 10 11 01
```

### Example 5: Delta Modulation
```
Analog samples: 1.0 1.5 1.2 0.8
Output: 1 1 0 0
```

## Program Flow

1. **Input Selection**: Choose between digital or analog input
2. **Data Entry**: Provide binary string or analog samples
3. **Encoding Choice**: Select line coding scheme
4. **Scrambling (Optional)**: Apply scrambling for AMI encoding
5. **Analysis**: Display analytical results (palindromes, zero sequences)
6. **Visualization**: OpenGL window shows the signal waveform
7. **Exit**: Close the OpenGL window to terminate

## Output Interpretation

### Console Output
- Encoded signal values (-1, 0, 1)
- Analytical results (longest palindrome, zero sequences)
- Status messages

### Graphical Output
- **X-axis**: Bit positions (or time for Manchester)
- **Y-axis**: Voltage levels (-1, 0, 1)
- **Grid**: Reference lines at +1, 0, -1 levels
- **Signal**: Blue line representing the encoded waveform
- **Labels**: Bit positions and voltage values

## File Structure

```
dc-assignment/
├── signal_generator.cpp    # Main source code
├── signal_generator.exe    # Compiled executable
├── libfreeglut.a          # Static FreeGLUT library
├── freeglut.dll           # Dynamic FreeGLUT library
├── GL/                    # OpenGL headers (if present)
└── README.md              # This file
```

## Dependencies

### External Libraries
- **FreeGLUT**: For OpenGL window management and input handling
- **OpenGL**: For 2D graphics rendering
- **GLU**: OpenGL Utility Library for perspective and camera functions

### Standard Libraries Used
- `<iostream>`: Console input/output
- `<cstring>`: String manipulation
- `<cmath>`: Mathematical functions
- `<GL/glut.h>`: GLUT header

## Known Issues and Limitations

- Maximum input length: 1000 bits (configurable)
- OpenGL window size fixed at 1200x700
- Requires graphics hardware support
- Console input only (no GUI for data entry)
- Windows-specific executable (may need recompilation for other OS)

## Troubleshooting

### Common Issues
1. **"freeglut.dll not found"**
   - Copy `freeglut.dll` to the executable directory or system path

2. **OpenGL window doesn't appear**
   - Check graphics drivers
   - Ensure OpenGL support is enabled

3. **Compilation errors**
   - Verify compiler version and library paths
   - Check for missing headers

4. **Program crashes on large inputs**
   - Reduce input size or increase array limits in code

### Debug Mode
Compile with debug flags for troubleshooting:
```
g++ signal_generator.cpp -o signal_generator.exe -lfreeglut -lglu32 -lopengl32 -g
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make changes and test thoroughly
4. Submit a pull request with detailed description


## Version History

- v1.0: Initial release with basic line coding and visualization
- Future versions may include additional encoding schemes and improved UI

---

**Note**: This README assumes a Windows environment. For Linux/Mac, adjust compilation commands and library installations accordingly.
