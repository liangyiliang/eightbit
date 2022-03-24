#/usr/env/bash

source=$1
customasm "${source}" -f hexcomma -o write/to_write.h
echo "Finished."