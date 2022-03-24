
# Compiles the C++ program
g++ control.cpp -o control
# Runs the C++ program to get the binary files
./control

# Uses Minipro to write the three EEPROMs with microcodes.
read -p "Place Control 1 and press Enter"
echo "*** Writing Control 1 ***"
minipro -p AT28C256 -w control_1.bin
echo "*** Finished Control 1 ***"
read -p "Place Control 2 and press Enter"
echo "*** Writing Control 2 ***"
minipro -p AT28C256 -w control_2.bin
echo "*** Finished Control 2 ***"
read -p "Place Control 3 and press Enter"
echo "*** Writing Control 3 ***"
minipro -p AT28C256 -w control_3.bin
echo "*** Finished Control 3 ***"
echo "*** ALL DONE! ***"
