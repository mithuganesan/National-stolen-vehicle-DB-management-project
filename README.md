Prerequisites:
==============
install CMake 3.2+, python(2.x), libboost-all-dev, libasio-dev.
git clone -b master https://github.com/mongodb/mongo-cxx-driver.
cd mongo-cxx-driver.
git checkout r3.0.1.
Download the release tarball: https://github.com/mongodb/mongo-c-driver/releases/download/1.3.5/mongo-c-driver-1.3.5.tar.gz.
tar xvf mongo-c-driver-1.3.5.tar.gz; cd mongo-c-driver-1.3.5; ./configure && make && sudo make install.
cd mongo-cxx-driver/build.
[PKG_CONFIG_PATH=/usr/local/lib/pkgconfig] cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local .. .
sudo make && sudo make install.


DB creation:
=============
Create a list using mockaroo.
mongoimport --db crossover --collection vehicles --drop --file Stolenvehicles.json.

Bringing up server:
====================
g++  -std=c++11 -g -I"/usr/local/include" webservice.cpp -o webservice -L"/usr/local/lib/"  -lrestbed -lbsoncxx $(pkg-config --cflags --libs libmongocxx).
./webservice.
