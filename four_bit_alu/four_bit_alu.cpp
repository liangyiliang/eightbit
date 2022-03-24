#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <stdint.h>
using namespace std;

#define OP_ADD 0b0000
#define OP_SUB 0b0001
#define OP_AND 0b0010
#define OP_ORA 0b0011
#define OP_NOT 0b0100
#define OP_EOR 0b0101
#define OP_LSL 0b0110
#define OP_LSR 0b0111
#define OP_ROL 0b1000
#define OP_ROR 0b1001
#define OP_INC 0b1010
#define OP_DEC 0b1011
#define OP_NOR 0b1100
#define OP_NAN 0b1101
#define OP_042 0b1110
#define OP_IDA 0b1111

struct ParsedAddress
{
    bool lsb_in;
    bool mode;
    bool msb_in;
    uint8_t opcode;
    uint8_t input_b;
    uint8_t input_a;
};

struct ParsedOutput
{
    uint8_t result;
    bool msb_out;
    bool lsb_out;
    bool zero;
};

struct ParsedAddress parse_address(uint16_t addr)
{
    ParsedAddress parsed;
    parsed.input_b = addr & 0b1111;
    parsed.opcode = (addr >> 4) & 0b1111;
    uint8_t temp = (addr >> 8) & 0b0011;
    temp |= ((addr >> 8) & 0b0100) << 1;
    temp |= ((addr >> 8) & 0b1000) >> 1;
    uint8_t temp2 = (temp & 0b0001) << 3;
    temp2 |= (temp & 0b1000) >> 3;
    temp2 |= (temp & 0b0100) >> 1;
    temp2 |= (temp & 0b0010) << 1;
    parsed.input_a = temp2;
    parsed.msb_in = (addr >> 12) & 1;
    parsed.mode = (addr >> 13) & 1;
    parsed.lsb_in = (addr >> 14) & 1;
    return parsed;
}

struct ParsedOutput map_address_to_output(uint16_t addr)
{
    auto parsed = parse_address(addr);
    ParsedOutput output;
    uint8_t temp;
    if (parsed.mode)
    { // if non least-significant nibble
        switch (parsed.opcode)
        {
        case OP_ADD:
            temp = parsed.input_a + parsed.input_b + parsed.lsb_in;
            output.result = temp & 0b1111;
            output.msb_out = (temp & 0b10000) >> 4;
            output.lsb_out = 0;
            break;

        case OP_SUB:
            temp = parsed.input_a + 
                (~parsed.input_b & 0b1111) + 1;
            if (!parsed.lsb_in) {
                temp += 0b00001110 + 1;
            }
            output.result = temp & 0b1111;
            output.msb_out = (temp & 0b10000) >> 4;
            output.lsb_out = 0;
            break;

        case OP_AND:
            temp = parsed.input_a & parsed.input_b;
            output.result = temp & 0b1111;
            output.msb_out = 0;
            output.lsb_out = 0;
            break;

        case OP_ORA:
            temp = parsed.input_a | parsed.input_b;
            output.result = temp & 0b1111;
            output.msb_out = 0;
            output.lsb_out = 0;
            break;

        case OP_NOT:
            temp = ~parsed.input_a;
            output.result = temp & 0b1111;
            output.msb_out = 0;
            output.lsb_out = 0;
            break;

        case OP_EOR:
            temp = parsed.input_a ^ parsed.input_b;
            output.result = temp & 0b1111;
            output.msb_out = 0;
            output.lsb_out = 0;
            break;

        case OP_LSL:
            temp = (parsed.input_a << 1) + parsed.lsb_in;
            output.result = temp & 0b1111;
            output.msb_out = (temp & 0b10000) >> 4;
            output.lsb_out = 0;
            break;

        case OP_LSR:
            temp = (parsed.input_a >> 1) | (parsed.msb_in << 3); 
            output.result = temp & 0b1111;
            output.msb_out = 0;
            output.lsb_out = parsed.input_a & 1;
            break;
            
        case OP_ROL:
            temp = (parsed.input_a << 1) + parsed.lsb_in;
            output.result = temp & 0b1111;
            output.msb_out = (temp & 0b10000) >> 4;
            output.lsb_out = 0;
            break;
            
        case OP_ROR:
            temp = (parsed.input_a >> 1) | (parsed.msb_in << 3); 
            output.result = temp & 0b1111;
            output.msb_out = 0;
            output.lsb_out = parsed.input_a & 1;
            break;

        case OP_INC:
            temp = parsed.input_a + parsed.lsb_in;
            output.result = temp & 0b1111;
            output.msb_out = (temp & 0b10000) >> 4;
            output.lsb_out = 0;
            break;
            
        case OP_DEC:
            temp = parsed.input_a;
            if (!parsed.lsb_in) {
                temp += 0b00001110 + 1;
            }
            output.result = temp & 0b1111;
            output.msb_out = (temp & 0b10000) >> 4;
            output.lsb_out = 0;
            break;
            
        case OP_NOR:
            temp = ~(parsed.input_a | parsed.input_b);
            output.result = temp & 0b1111;
            output.msb_out = 0;
            output.lsb_out = 0;
            break;
            
        case OP_NAN:
            temp = ~(parsed.input_a & parsed.input_b);
            output.result = temp & 0b1111;
            output.msb_out = 0;
            output.lsb_out = 0;
            break;

        case OP_042:
            if (parsed.lsb_in)
                temp = 0;
            else
                temp = 0b0010;
            output.result = temp;
            output.msb_out = 1;
            output.lsb_out = 0;
            break;

        case OP_IDA:
            temp = parsed.input_a;
            output.result = temp & 0b1111;
            output.msb_out = 0;
            output.lsb_out = 0;
            break;
        }
    }
    else // least-significant nibble
    {
        switch (parsed.opcode)
        {
        case OP_ADD:
            temp = parsed.input_a + parsed.input_b;
            output.result = temp & 0b1111;
            output.msb_out = (temp & 0b10000) >> 4;
            output.lsb_out = 0;
            break;

        case OP_SUB:
            temp = parsed.input_a + (~parsed.input_b & 0b1111) + 1;
            output.result = temp & 0b1111;
            output.msb_out = (temp & 0b10000) >> 4;
            output.lsb_out = 0;
            break;

        case OP_AND:
            temp = parsed.input_a & parsed.input_b;
            output.result = temp & 0b1111;
            output.msb_out = 0;
            output.lsb_out = 0;
            break;

        case OP_ORA:
            temp = parsed.input_a | parsed.input_b;
            output.result = temp & 0b1111;
            output.msb_out = 0;
            output.lsb_out = 0;
            break;

        case OP_NOT:
            temp = ~parsed.input_a;
            output.result = temp & 0b1111;
            output.msb_out = 0;
            output.lsb_out = 0;
            break;

        case OP_EOR:
            temp = parsed.input_a ^ parsed.input_b;
            output.result = temp & 0b1111;
            output.msb_out = 0;
            output.lsb_out = 0;
            break;

        case OP_LSL:
            temp = parsed.input_a << 1;
            output.result = temp & 0b1111;
            output.msb_out = (temp & 0b10000) >> 4;
            output.lsb_out = 0;
            break;

        case OP_LSR:
            temp = (parsed.input_a >> 1) | (parsed.msb_in << 3); 
            output.result = temp & 0b1111;
            output.msb_out = 0;
            output.lsb_out = 0;
            break;
            
        case OP_ROL:
            temp = (parsed.input_a << 1) + parsed.lsb_in;
            output.result = temp & 0b1111;
            output.msb_out = (temp & 0b10000) >> 4;
            output.lsb_out = 0;
            break;
            
        case OP_ROR:
            temp = (parsed.input_a >> 1) | (parsed.msb_in << 3); 
            output.result = temp & 0b1111;
            output.msb_out = 0;
            output.lsb_out = parsed.input_a & 1;
            break;

        case OP_INC:
            temp = parsed.input_a + 1;
            output.result = temp & 0b1111;
            output.msb_out = (temp & 0b10000) >> 4;
            output.lsb_out = 0;
            break;
            
        case OP_DEC:
            temp = parsed.input_a + 0b00001110 + 1;
            output.result = temp & 0b1111;
            output.msb_out = (temp & 0b10000) >> 4;
            output.lsb_out = 0;
            break;
            
        case OP_NOR:
            temp = ~(parsed.input_a | parsed.input_b);
            output.result = temp & 0b1111;
            output.msb_out = 0;
            output.lsb_out = 0;
            break;
            
        case OP_NAN:
            temp = ~(parsed.input_a & parsed.input_b);
            output.result = temp & 0b1111;
            output.msb_out = 0;
            output.lsb_out = 0;
            break;

        case OP_042:
            temp = 0b1010;
            output.result = temp;
            output.msb_out = 0;
            output.lsb_out = 0;
            break;

        case OP_IDA:
            temp = parsed.input_a;
            output.result = temp & 0b1111;
            output.msb_out = 0;
            output.lsb_out = 0;
            break;
        }
    }
    output.zero = output.result == 0;
    return output;
}

uint8_t unparse_output(ParsedOutput parsed){
    uint8_t value = 0;
    value += (parsed.result & 0b1111) << 4;
    value += (parsed.msb_out & 1) << 1;
    value += parsed.lsb_out & 1;
    value += (parsed.zero & 1) << 2;
    return value;
}

void out_binary()
{
    ofstream outfile;
    outfile.open("alu.bin", ios::binary | ios::out);
    for (uint16_t addr = 0; addr < 32768; ++addr)
    {
        auto parsed_output = map_address_to_output(addr);
        auto value = unparse_output(parsed_output);
        outfile.put((char)value);
    }
    outfile.close();
}

int main()
{
    out_binary();
    return 0;
}

