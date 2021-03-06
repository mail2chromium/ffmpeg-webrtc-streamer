DEST="./build/"
echo "$DEST"

if [ -d "$DEST" ]; then
#  if [ -L "$DEST" ]; then
    # It is a symbolic links #
    echo "Directory found. Lets make it and proceed..."
    cd build
    make

else
    # It is a directory #
    echo "Directory not found. Lets proceed..."
    mkdir build && cd build
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
    make
#  fi
fi