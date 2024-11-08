#include "header.h"
#include <bits/stdc++.h>
using namespace std;

int main() {
    memory memoryObj;
    Register registerObj;
    CPU cpu(registerObj, memoryObj);

    cout << "Welcome to Vole Machine\n";
    cout << "Choose input method:\n";
    cout << "1. Load instructions from a file\n";
    cout << "2. Enter instructions manually\n";
    cout << "Enter your choice (1 or 2): ";

    int choice;
    cin >> choice;

    if (choice == 1) {
        // Load instructions from a file
        cout << "Enter the file name: ";
        string filename;
        cin >> filename;
        memoryObj.loadProgramFile(filename);
        cout << "Program loaded from file.\n";
    }
    else if (choice == 2) {
        // Manually enter instructions
        cout << "Enter instructions in hexadecimal format (e.g., '10FF' for 2 byte). Type 'end' to finish:\n";
        string instruction;
        int address = 10; // Starting address for instructions

        while (true) {
            cout << "Instruction at address " << address << ": ";
            cin >> instruction;
            if (instruction == "end") break;

            // Validate input for a single byte (2 hex characters)
            if (instruction.size() != 4 ||!memoryObj.isValidHex(instruction) ) {
                    cout << "Invalid format. Please enter a 4-character hex code.\n";
                    continue;

            }
            memoryObj.setCell(address, instruction.substr(0,1));
            memoryObj.setCell(address+1, instruction.substr(2,3));
            address+=2; // Move to the next address
        }
        cout << "Instructions added manually.\n";
    }
    else {
        cout << "Invalid choice. Exiting program.\n";
        return 1;
    }

    // Execute the program
    cpu.execute();
    cout << "Output screen:\n";
    if (memoryObj.getCell(0) != "00") {
        string hexValue = memoryObj.getCell(0); // Get the value in hexadecimal
        int decimalValue = stoi(hexValue, nullptr, 16); // Convert to decimal
        char c = static_cast<char>(decimalValue);
        cout << "hex: " << hexValue << "\n" << "ascii: " << c << "\n"; // Output in hex and ASCII
    }

    // Display memory and register contents
    memoryObj.outputMemory(memoryObj);
    registerObj.outputRegister(registerObj);

    return 0;
}
