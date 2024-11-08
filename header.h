#ifndef UNTITLED41_HEADER_H
#define UNTITLED41_HEADER_H
#include <bits/stdc++.h>
using namespace std;
static int pc = 10;


class Register {
private:
    array <string , 16>register_container;
public:
    Register();
    void setCell(int address, string instruction);
    string getCell(int address);
    void outputRegister(Register &registerObj);

};

class memory {
private:
    vector<string>memory_container;

public:
    memory();
    void setCell(long address, const string &instruction);
    string getCell(long address);
    void loadProgramFile(const string &filename);
    bool isValidHex(const string &str);
    void outputMemory(memory &memoryObj);

};

class CPU{
private:

    int pc = 10;
    string IR;
    Register &reg;
    memory &mem;
    // Helper functions for hexadecimal and decimal conversions
    int hexToDecimal( const string &hex);
    string decimalToHex(int dec);
    void add(int n, int r, int s, int t, Register&registerObj);
public:

    CPU(Register &registerObj, memory &memoryObj);
    void fetch(memory &memoryObj);
    void load(int reg, int memAddress, Register &registerObj, memory &memory);
    void store(int reg, int memAddress, Register &registerObj, memory &memory);
    void move(int reg_r, int reg_s, Register &registerObj);
    void jump(int reg, const string &memAddress, Register &registerObj);
    void halt();
    bool haltFlag= false;

    void execute();


};



#endif
