#include <bits/stdc++.h>
#include "header.h"
using namespace std;

memory::memory() {
    memory_container = vector<string>(265, "00");
}

void memory::setCell(long address, const string &instruction){
    if (address >= 0 && address < memory_container.size()) {
        memory_container[address] = instruction;
    } else {
        cerr << "Error: Address out of bounds" << endl;
    }
}
string memory::getCell(long address)  {
    if (address >= 0 && address < memory_container.size()) {
        return memory_container[address];
    } else {
        cerr << "Error: Address out of bounds" << endl;
        return "";
    }
}
void memory::outputMemory(memory &memoryObj) {
    cout<<"memory : \n";
    cout << "    ";
    for (int i = 0; i < 16; i++) {
        cout << hex << uppercase << i << "  ";
    }
    cout << endl;

    for (int j = 0; j < 16; j++) {
        cout  << hex << uppercase << j << " : ";
        for (int m = 0; m < 16; m++) {
            cout << memoryObj.getCell(j * 16 + m) << " ";
        }
        cout << endl;
    }
}
bool memory::isValidHex(const string &str) {
    for (char digit : str) {
        if (!((digit >= '0' && digit <= '9') ||
              (digit >= 'A' && digit <= 'F') ||
              (digit >= 'a' && digit <= 'f'))) {

            return false;
        }
    }
    return true;
}
void memory::loadProgramFile(const string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    long address = 10;
    string instruction;
    while (file >> instruction) {
        if (instruction.length() == 4) {
            if(!isValidHex(instruction)){
                cout<<"wrong hexadecimal number !!"<<endl;
                continue;
            }
            string s1 = instruction.substr(0, 2);
            string s2 = instruction.substr(2, 2);
            if (address < 265) {
                setCell(address++, s1);
            }
            if (address < 265) {
                setCell(address++, s2);
            }
        } else {
            break;
        }
    }
    file.close();
}

Register::Register() {
    for (int i=0;i<16;i++){
        setCell(i,"00") ;
    }
}

void Register::setCell(int address, string instruction) {
    register_container[address]=instruction;
}

string Register:: getCell(int address){
    return register_container[address];
}

void Register::outputRegister(Register &registerObj) {
    cout<<"Register: \n";
    cout << "    ";
    for (int i = 0; i < 16; i++) {
        cout << hex << uppercase << registerObj.getCell(i)<< "  ";
    }
    cout << endl;
}
CPU::CPU(Register& registerObj, memory& memoryObj)
        : reg(registerObj), mem(memoryObj), haltFlag(false) { // Initialize haltFlag to false
}

// Helper function to convert hexadecimal to decimal
int CPU::hexToDecimal( const string &hex) {
    int decimal = 0;
    int base = 1;
    for (int i = hex.length() - 1; i >= 0; i--) {
        char digit = hex[i];
        int value;
        if (isdigit(digit)) {
            value = digit - '0';
        } else if (isupper(digit)) {
            value = digit - 'A' + 10;
        } else if (islower(digit)) {
            value = digit - 'a' + 10;
        } else {
            throw invalid_argument("Invalid hexadecimal character");
        }
        decimal += value * base;
        base *= 16;
    }
    return decimal;
}

void CPU::add(int n, int r, int s, int t, Register &registerObj) {
    if (r < 0 || r >= 16 || s < 0 || s >= 16 || t < 0 || t >= 16) {
        cerr << "Error: Register index out of bounds." << endl;
        return;
    }

    if (n == 5) {  // Integer addition
        // Integer addition
        // Assume reg.getCell(s) and reg.getCell(t) provide integer values from a register
        int value_s = stoi(registerObj.getCell(s), nullptr, 16);
        int value_t = stoi(registerObj.getCell(t), nullptr, 16);

        // Convert integer values to 8-bit binary strings
        string binary_s = bitset<8>(value_s).to_string(); // Convert value_s to binary
        string binary_t = bitset<8>(value_t).to_string(); // Convert value_t to binary

        // Convert binary strings to integers for addition
        int num_s = stoi(binary_s, nullptr, 2); // Convert binary_s to integer
        int num_t = stoi(binary_t, nullptr, 2); // Convert binary_t to integer
        int total = num_s + num_t; // Perform binary addition
        // Convert the result to hexadecimal
        stringstream ss;
        ss << hex << uppercase << total; // Convert total to hexadecimal in uppercase
        string hex_result = ss.str();

        // Display the hexadecimal result
        cout << "Hexadecimal Addition Result: " << hex_result << endl;
        registerObj.setCell(r, ss.str());

    }
    else if (n == 6) {  // Floating-point addition
        int value_s = stoi(registerObj.getCell(s), nullptr, 16);
        int value_t = stoi(registerObj.getCell(t), nullptr, 16);

        // Extract the exponent (upper 4 bits) and mantissa (lower 4 bits)
        int exponent_s = (value_s >> 4) & 0x0F;
        int mantissa_s = value_s & 0x0F;

        int exponent_t = (value_t >> 4) & 0x0F;
        int mantissa_t = value_t & 0x0F;

        // Align exponents
        if (exponent_s > exponent_t) {
            mantissa_t >>= (exponent_s - exponent_t);
            exponent_t = exponent_s;
        } else if (exponent_t > exponent_s) {
            mantissa_s >>= (exponent_t - exponent_s);
            exponent_s = exponent_t;
        }

        // Add mantissas
        int mantissa_total = mantissa_s + mantissa_t;
        int exponent_total = exponent_s;

        // Adjust if mantissa exceeds 4 bits
        if (mantissa_total >= 16) {  // 16 is 2^4, so we shift and adjust exponent
            mantissa_total >>= 1;
            exponent_total += 1;
        }

        // Combine exponent and mantissa
        int result = ((exponent_total & 0x0F) << 4) | (mantissa_total & 0x0F);

        // Convert result to hexadecimal
        stringstream ss;
        ss << hex << uppercase << result;
        cout << "Floating-Point Addition Result: " << ss.str() << endl;
        registerObj.setCell(r, ss.str());
    }
}

// Load from memory
void CPU::load(int reg, int memAddress, Register &registerObj, memory &memoryObj) {
    string value = memoryObj.getCell(memAddress);
    registerObj.setCell(reg, value);
}

// Store into memory
void CPU::store(int reg, int memAddress, Register &registerObj, memory &memoryObj) {
    string value = registerObj.getCell(reg);
    memoryObj.setCell(memAddress, value);
}


// Move data between registers
void CPU::move(int reg_r, int reg_s, Register &registerObj) {
    string value = registerObj.getCell(reg_r);
    registerObj.setCell(reg_s, value);
}

// Jump to a specific memory address if condition is met
void CPU::jump(int reg, const string &memAddress, Register &registerObj) {
    int addressJump = hexToDecimal(memAddress);
    if (registerObj.getCell(reg) == registerObj.getCell(0)) {
        pc = addressJump;
    }

}

void CPU::halt() {
    cout << "Execution halted." << endl;
    haltFlag = true;  // Set haltFlag to true
}
string CPU::decimalToHex(int dec) {
    if (dec == 0) return "0";
    string hex;
    while (dec > 0) {
        int remainder = dec % 16;
        hex = (remainder < 10 ? char(remainder + '0') : char(remainder - 10 + 'A')) + hex;
        dec /= 16;
    }
    return hex;
}
//helper function
int bitsetToIntTwosComplement(std::bitset<8> bits) {
    // If the MSB is 1, the number is negative in two's complement
    if (bits[7] == 1) {
        // Two's complement: negate and add 1
        return static_cast<int>(bits.to_ulong()) - 256;
    } else {
        return static_cast<int>(bits.to_ulong());
    }
}
//memory &memoryObj,Register &registerObj,machine &machineObj
void CPU::execute() {
    while (!haltFlag) { // Check haltFlag each cycle
        IR = mem.getCell(pc) + mem.getCell(pc + 1);  // Fetch the instruction
        char opcode = IR[0];
        switch (opcode) {
            case '1': {  // Load from memory
                int regInt = hexToDecimal(IR.substr(1, 1));
                int memAddress = hexToDecimal(IR.substr(2, 2));
                load(regInt, memAddress, reg, mem);
                break;
            }
            case '2': {  // Load a pattern into a register
                int regInt = hexToDecimal(IR.substr(1, 1));
                string pattern = IR.substr(2, 2);
                reg.setCell(regInt, pattern);
                break;
            }
            case '3': {  // Store into memory
                int regInt = hexToDecimal(IR.substr(1, 1));
                int memAddress = hexToDecimal(IR.substr(2, 2));
                store(regInt, memAddress, reg, mem);
                break;
            }
            case '4': {  // Move data between registers
                int reg_r = hexToDecimal(IR.substr(2, 1));
                int reg_s = hexToDecimal(IR.substr(3, 1));
                move(reg_r, reg_s, reg);
                break;
            }
            case '5':  // Integer addition
            case '6': {  // Floating-point addition
                int n = opcode - '0';  // Get the operation type from opcode
                int r = hexToDecimal(IR.substr(1, 1));
                int s = hexToDecimal(IR.substr(2, 1));
                int t = hexToDecimal(IR.substr(3, 1));
                add(n, r, s, t, reg);
                break;
            }
            case '7': {
                int reg1 = hexToDecimal(IR.substr(2, 1));
                int reg2 = hexToDecimal(IR.substr(3, 1));

                // Convert the string values in the registers to integers
                int value1 = hexToDecimal(reg.getCell(reg1));
                int value2 = hexToDecimal(reg.getCell(reg2));

                // Perform bitwise OR on the integer values
                int result = value1 | value2;

                // Optionally store the result back in a register or output it
                reg.setCell(reg1, decimalToHex(result));
                break;
            }
            case '8': {
                int reg1 = hexToDecimal(IR.substr(2, 1));
                int reg2 = hexToDecimal(IR.substr(3, 1));

                // Convert the string values in the registers to integers
                int value1 = hexToDecimal(reg.getCell(reg1));
                int value2 = hexToDecimal(reg.getCell(reg2));

                // Perform bitwise And on the integer values
                int result = value1 & value2;

                // Optionally store the result back in a register or output it
                reg.setCell(reg1, decimalToHex(result));
                break;
            }
            case '9': {
                int reg1 = hexToDecimal(IR.substr(2, 1));
                int reg2 = hexToDecimal(IR.substr(3, 1));

                // Convert the string values in the registers to integers
                int value1 = hexToDecimal(reg.getCell(reg1));
                int value2 = hexToDecimal(reg.getCell(reg2));

                // Perform bitwise XOR on the integer values
                int result = value1 ^ value2;

                // Optionally store the result back in a register or output it
                reg.setCell(reg1, decimalToHex(result));
                break;
            }
            case 'A': {
                int reg1 = hexToDecimal(IR.substr(1, 1));//register address
                int steps = hexToDecimal(IR.substr(3, 1));//number of steps i need to rotate

                int value = hexToDecimal(reg.getCell(reg1));
                bitset<8> binary_value(value);
                binary_value >> steps;
                // Convert the bitset to an unsigned long using the built-in `to_ulong()`
                unsigned long decimal_value = binary_value.to_ulong();

                // Convert the decimal value to a hexadecimal string using a stringstream
                string hex_value = decimalToHex(decimal_value);
                reg.setCell(reg1, hex_value);
            }
            case 'B': {  // Jump to memory address
                int regInt = hexToDecimal(IR.substr(1, 1));
                string memAddress = IR.substr(2, 2);
                jump(regInt, memAddress, reg);
                break;
            }
            case 'C': {  // Halt execution
                halt();
                return;
            }
            case 'D': {
                int registernum= hexToDecimal(IR.substr(1,1));
                int regR = hexToDecimal( reg.getCell(registernum));
                int reg0 = hexToDecimal(reg.getCell(0));  // Value of register 0

                // Convert the bitsets to signed integers (two's complement)
                int valueR = bitsetToIntTwosComplement(regR);
                int value0 = bitsetToIntTwosComplement(reg0);

                // Compare the values
                if (valueR > value0) {
                    int memoryCellXY = hexToDecimal(IR.substr(2,2));
                    pc = memoryCellXY;
                }
                }
                default:
                    cout << "Error: Invalid opcode!" << endl;
                halt();
                 }
                pc += 2;
            }

 }