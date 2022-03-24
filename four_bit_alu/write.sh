g++ four_bit_alu.cpp -o alu
./alu
read -p "Place EEPROM and press Enter"
echo "*** Writing ALU to EEPROM ***"
minipro -p AT28C256 -w alu.bin
echo "*** DONE ***"