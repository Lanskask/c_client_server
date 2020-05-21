echo "Building code"
rm -r build
mkdir build
cd build
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build .

echo "Running"
nohup ./server &
echo '!!! DONT FORGET TO KILL server'
./client