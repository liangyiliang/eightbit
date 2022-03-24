#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <stdint.h>
using namespace std;

/*
#define HL 0b000000010000000000000000 // Halt
#define RI 0b000000100000000000000000 // RAM In
#define CE 0b000001000000000000000000 // Counter Enable
#define SU 0b000010000000000000000000 // ALU Subtract
#define MI 0b100000000000000000000000 // MAR In
#define II 0b100100000000000000000000 // Instruction Register In
#define CI 0b101000000000000000000000 // Counter In
#define AI 0b101100000000000000000000 // A Register In
#define BI 0b110000000000000000000000 // B Register In
#define OI 0b110100000000000000000000 // Output Register In
#define DI 0b111000000000000000000000 // Display Register In
#define SI 0b111100000000000000000000 // Stack Register In
#define RO 0b000000000000100000000000 // RAM Out
#define CO 0b000000000000100100000000 // Counter Out
#define AO 0b000000000000101000000000 // A Register Out
#define EO 0b000000000000101100000000 // ALU Out
#define BO 0b000000000000110000000000 // B Register Out
#define SO 0b000000000000110100000000 // Stack Register Out
#define IO 0b000000000000111000000000 // Input Module Out
#define DO 0b000000000000111100000000 // RanDom Module Out
#define FI 0b000000000001000000000000 // Flags In
#define DR 0b000000000010000000000000 // Display Register Select
#define DE 0b000000000100000000000000 // Display Enable
#define NT 0b000000000000000000000001 // Nothing
#define SP 0b000000000000000000000010 // Stack Register Increment
#define SD 0b000000000000000000000100 // Stack Register Decrement
#define SA 0b000000000000000000001000 // Stack Access

*/

#define HL 0b000000010000000000000000 // Halt                           Stops the clock
#define CE 0b000000100000000000000000 // Counter Enable                 Increments Program Counter
#define FI 0b000001000000000000000000 // Flags IN                       Sets the Flags register based on ALU result
#define SU 0b000010000000000000000000 // ALU Subtract                   ALU subtracts instead of adds
#define MI 0b000000000001000000000000 // MAR IN                         Sets the Memory Address Register (MAR)
#define RI 0b000000000010000000000000 // RAM IN                         Sets the RAM content at address in MAR
#define II 0b000000000011000000000000 // Instruction Register IN        Sets the Instruction Register
#define CI 0b000000000100000000000000 // Counter IN                     Sets the Program Counter (for jumps)
#define AI 0b000000000101000000000000 // Accumulator IN                 Sets the accumulator
#define BI 0b000000000110000000000000 // B Register IN                  Sets the B register
#define OI 0b000000000111000000000000 // Output Reigster IN             Sets the 7-segment display register
#define SI 0b000000001000000000000000 // Stack Register IN              Sets the stack point register
#define RO 0b000000000000000100000000 // RAM OUT                        Outputs the RAM content at address in MAR
#define CO 0b000000000000001000000000 // Counter OUT                    Outputs the Program Counter content
#define AO 0b000000000000001100000000 // Accumulator OUT                Outputs the content of Accumulator
#define EO 0b000000000000010000000000 // ALU Out                        Outputs the current ALU result
#define BO 0b000000000000010100000000 // B Register Out                 Outputs the B register content
                                      //                                haven't found a use for that yet
#define SO 0b000000000000011000000000 // Stack Register OUT             Outputs the stack point register content
#define PO 0b000000000000011100000000 // disPlay OUT                    Outputs the LCD Display Register Content

#define IO 0b000000000000100000000000 // Input OUT                      Outputs the content in the Input register
#define DO 0b000000000000100100000000 // ranDom OUT                     Outputs the value in the quick-n-dirty random-bit generator

#define PR 0b010000000000000000000000 // disPlay Register select        Selects which LCD display register to access
#define PE 0b100000000000000000000000 // disPlay Enable                 LCD Display Enable

#define NT 0b000000000000000000000001 // NoThing                        Skips to the next instruction to save cycle time.
#define SP 0b000000000000000000000010 // Stack Register Increment       Increments the Stack Point Register
#define SD 0b000000000000000000000100 // Stack Register Decrement       Decrements the Stack Point Register
#define SA 0b000000000000000000001000 // Stack Access                   Indicates whether or not to access the Stack portion of the RAM

#define NUM_AVAILABLE_OPCODES 256
#define NUM_STEPS 8

uint32_t ucode_template[NUM_AVAILABLE_OPCODES][NUM_STEPS] = {
    {CO | MI, RO | II | CE, NT, 0, 0, 0, 0},                                           // 00000000 - NOP
    {CO | MI, RO | II | CE, CO | MI, RO | MI | CE, RO | AI, NT, 0, 0},                 // 00000001 - LDA [addr]
    {CO | MI, RO | II | CE, CO | MI, RO | MI | CE, RO | BI, EO | AI | FI, NT, 0},      // 00000010 - ADD [addr]
    {CO | MI, RO | II | CE, CO | MI, RO | MI | CE, RO | BI, EO | AI | SU | FI, NT, 0}, // 00000011 - SUB [addr]
    {CO | MI, RO | II | CE, CO | MI, RO | MI | CE, AO | RI, NT, 0, 0},                 // 00000100 - STA [addr]
    {CO | MI, RO | II | CE, CO | MI, RO | AI | CE, NT, 0, 0, 0},                       // 00000101 - LDA.I [value]
    {CO | MI, RO | II | CE, CO | MI, RO | CI, NT, 0, 0, 0},                            // 00000110 - JMP [addr]
    {CO | MI, RO | II | CE, CE, NT, 0, 0, 0, 0},                                       // 00000111 - JMC [addr]
    {CO | MI, RO | II | CE, CE, NT, 0, 0, 0, 0},                                       // 00001000 - JMZ [addr]
    {CO | MI, RO | II | CE, CO | MI, RO | BI | CE, EO | AI | FI, NT, 0, 0},            // 00001001 - ADD.I [value]
    {CO | MI, RO | II | CE, CO | MI, RO | BI | CE, EO | AI | SU | FI, NT, 0, 0},       // 00001010 - SUB.I [value]
    {CO | MI, RO | II | CE, CO | MI, RO | BI | CE, EO | MI, RO | AI, NT, 0},           // 00001011 - LDA.REL [offset]
    {CO | MI, RO | II | CE, CE, NT, 0, 0, 0, 0},                                       // 00001100 - JMN
    {CO | MI, RO | II | CE, NT, 0, 0, 0, 0},                                           // 00001101 - NOP
    {CO | MI, RO | II | CE, AO | OI, NT, 0, 0, 0, 0},                                  // 00001110 - OUT
    {CO | MI, RO | II | CE, HL, 0, 0, 0, 0, 0},                                        // 00001111 - HLT
    {CO | MI, RO | II | CE, NT, 0, 0, 0, 0, 0},                                        // 00010000 - NOP
    {CO | MI, RO | II | CE, AO, AO | PE, AO, NT, 0, 0},                                // 00010001 - TAPI - Put Reg A to Display Instruction and Toggle Enable
    {CO | MI, RO | II | CE, AO | PR, AO | PR | PE, AO | PR, NT, 0, 0},                 // 00010010 - TAPD - Put Reg A to Display Data and Toggle Enable
    {CO | MI, RO | II | CE, SO | MI, AO | RI | SA | SP, NT, 0, 0, 0},                  // 00010011 - PSH - Push Reg A to Stack
    {CO | MI, RO | II | CE, SD, SO | MI, SA | RO | AI, NT, 0, 0},                      // 00010100 - POP - Pop Stack onto Reg A
    {CO | MI, RO | II | CE, SO | AI, NT, 0, 0, 0, 0},                                  // 00010101 - TSA - Loads Reg A with Stack Register Content
    {CO | MI, RO | II | CE, NT, 0, 0, 0, 0, 0},                                        // 00010110 - NOP
    {CO | MI, RO | II | CE, AO | SI, NT, 0, 0, 0, 0},                                  // 00010111 - TAS - Loads Stack Register with Reg A
    {CO | MI, RO | II | CE, CO | MI, RO | MI | CE, RO | BI, SU | FI, NT, 0},           // 00011000 - SUBF [addr] - Sets the flags for subtracting (Reg A - MEM[addr])
    {CO | MI, RO | II | CE, CO | MI, RO | BI | CE, SU | FI, NT, 0, 0},                 // 00011001 - SUBF.I [value] - Sets the flags for subtraction (Reg A - value)
    {CO | MI, RO | II | CE, SO | MI, CO | RI | SA | SP, CO | MI, RO | CI, NT, 0},      // 00011010 - JSR - Jump to SubRoutine
    {CO | MI, RO | II | CE, SD, SO | MI, RO | CI | SA, CE, NT, 0},                     // 00011011 - RTS - ReTurn from Subroutine
    {CO | MI, RO | II | CE, IO | AI, NT, 0, 0, 0, 0},                                  // 00011100 - TIA - Loads Reg A with content of Input Module
    {CO | MI, RO | II | CE, DO | AI, NT, 0, 0, 0, 0},                                  // 00011101 - TRA - Loads Reg A with content of Random Module
    {CO | MI, RO | II | CE, CO | MI, RO | CE, RO | PE, RO, NT, 0},                     // 00011110 - LDPI.I - Loads Display Instruction with Value and Toggle Enable
    {CO | MI, RO | II | CE, CO | MI, RO | PR | CE, RO | PR | PE, RO | PR, NT, 0},      // 00011111 - LDPD.I - Loads Display Data with Value and Toggle Enable
    {CO | MI, RO | II | CE, PO, PO | PE, PO | PE | AI, PO, NT, 0},                     // 00100000 - TPIA - Loads Reg A with Display Busy Flag and Address
    {CO | MI, RO | II | CE, PO | PR, PO | PR | PE, PO | PR | PE | AI, PO | PR, NT, 0}, // 00100001 - TPDA - Loads Reg A with Display Data
};
// ***  UPDATE HERE         vv
#define NUM_DEFINED_OPCODES 34

#define JMC 0b00000111
#define JMZ 0b00001000
#define JMN 0b00001100
#define F_N0ZR0C0 0
#define F_N0ZR0C1 1
#define F_N0ZR1C0 2
#define F_N0ZR1C1 3
#define F_N1ZR0C0 4
#define F_N1ZR0C1 5
#define F_N1ZR1C0 6
#define F_N1ZR1C1 7

uint32_t ucode[16][NUM_AVAILABLE_OPCODES][NUM_STEPS];


void init_template()
{
    // All other commands are NOP
    for (size_t i = NUM_DEFINED_OPCODES; i < NUM_AVAILABLE_OPCODES; i++)
    {
        memset(ucode_template[i], 0, sizeof(ucode_template[i]));
        ucode_template[i][0] = CO | MI;
        ucode_template[i][1] = RO | II | CE;
        ucode_template[i][2] = CE;
        ucode_template[i][3] = NT;
    }
}

void init_ucode()
{
    // Generates the full microcode that considers each flags register scenario.
    // Will have to be optimized as we have more and more flags.
    init_template();

    // N = 0, ZR = 0, C = 0
    memcpy(ucode[F_N0ZR0C0], ucode_template, sizeof(ucode_template));

    // N = 0, ZR = 0, C = 1
    memcpy(ucode[F_N0ZR0C1], ucode_template, sizeof(ucode_template));
    ucode[F_N0ZR0C1][JMC][2] = CO | MI;
    ucode[F_N0ZR0C1][JMC][3] = RO | CI;
    ucode[F_N0ZR0C1][JMC][4] = NT;

    // N = 0, ZR = 1, C = 0
    memcpy(ucode[F_N0ZR1C0], ucode_template, sizeof(ucode_template));
    ucode[F_N0ZR1C0][JMZ][2] = CO | MI;
    ucode[F_N0ZR1C0][JMZ][3] = RO | CI;
    ucode[F_N0ZR1C0][JMZ][4] = NT;

    // N = 0, ZR = 1, C = 1
    memcpy(ucode[F_N0ZR1C1], ucode_template, sizeof(ucode_template));
    ucode[F_N0ZR1C1][JMC][2] = CO | MI;
    ucode[F_N0ZR1C1][JMC][3] = RO | CI;
    ucode[F_N0ZR1C1][JMC][4] = NT;
    ucode[F_N0ZR1C1][JMZ][2] = CO | MI;
    ucode[F_N0ZR1C1][JMZ][3] = RO | CI;
    ucode[F_N0ZR1C1][JMZ][4] = NT;

    // N = 1, ZR = 0, C = 0
    memcpy(ucode[F_N1ZR0C0], ucode_template, sizeof(ucode_template));
    ucode[F_N1ZR0C0][JMN][2] = CO | MI;
    ucode[F_N1ZR0C0][JMN][3] = RO | CI;
    ucode[F_N1ZR0C0][JMN][4] = NT;

    // N = 1, ZR = 0, C = 1
    memcpy(ucode[F_N1ZR0C1], ucode_template, sizeof(ucode_template));
    ucode[F_N1ZR0C1][JMC][2] = CO | MI;
    ucode[F_N1ZR0C1][JMC][3] = RO | CI;
    ucode[F_N1ZR0C1][JMC][4] = NT;
    ucode[F_N1ZR0C1][JMN][2] = CO | MI;
    ucode[F_N1ZR0C1][JMN][3] = RO | CI;
    ucode[F_N1ZR0C1][JMN][4] = NT;

    // N = 1, ZR = 1, C = 0
    memcpy(ucode[F_N1ZR1C0], ucode_template, sizeof(ucode_template));
    ucode[F_N1ZR1C0][JMZ][2] = CO | MI;
    ucode[F_N1ZR1C0][JMZ][3] = RO | CI;
    ucode[F_N1ZR1C0][JMZ][4] = NT;
    ucode[F_N1ZR1C0][JMN][2] = CO | MI;
    ucode[F_N1ZR1C0][JMN][3] = RO | CI;
    ucode[F_N1ZR1C0][JMN][4] = NT;

    // N = 1, ZR = 1, C = 1
    memcpy(ucode[F_N1ZR1C1], ucode_template, sizeof(ucode_template));
    ucode[F_N1ZR1C1][JMC][2] = CO | MI;
    ucode[F_N1ZR1C1][JMC][3] = RO | CI;
    ucode[F_N1ZR1C1][JMC][4] = NT;
    ucode[F_N1ZR1C1][JMZ][2] = CO | MI;
    ucode[F_N1ZR1C1][JMZ][3] = RO | CI;
    ucode[F_N1ZR1C1][JMZ][4] = NT;
    ucode[F_N1ZR1C1][JMN][2] = CO | MI;
    ucode[F_N1ZR1C1][JMN][3] = RO | CI;
    ucode[F_N1ZR1C1][JMN][4] = NT;

    for (int i = 8; i < 16; i++)
    {
        memcpy(ucode[i], ucode[i & 0b111], sizeof(ucode_template));
    }
}

#define EEPROM_SIZE 32768

void out_binary(int num)
{
    // Outputs the ucode into three binary files
    // to be written to the EEPROMs.
    ofstream outfile;
    outfile.open("control_" + to_string(num) + ".bin", ios::binary | ios::out);
    init_ucode();
    for (uint32_t addr = 0; addr < EEPROM_SIZE; ++addr)
    {
        uint32_t flags = addr >> 11;
        uint32_t step = (addr >> 8) & 0b111;
        uint32_t opcode = (addr)&0b11111111;
        uint8_t to_write;
        if (num == 1)
        {
            to_write = ucode[flags][opcode][step] >> 16;
        }
        else if (num == 2)
        {
            to_write = (ucode[flags][opcode][step] >> 8) & 0b11111111;
        }
        else
        {
            to_write = ucode[flags][opcode][step] & 0b11111111;
        }

        outfile.put((char)to_write);
    }
    outfile.close();
}

int main()
{
    out_binary(1);
    out_binary(2);
    out_binary(3);
    return 0;
}