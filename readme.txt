skygrazer

Listens to the ADS-B json feed and displays a pretty image of the nearest
aircraft. Install dependencies, build, copy skygrazer.ini to /etc and
take off.

Splash screen from Wikipedia user Arpingstone.


Dependencies:

; jansson
wget http://www.digip.org/jansson/releases/jansson-2.7.tar.gz
tar xzf jansson-2.7.tar.gz
cd jansson-2.7
./configure && make && sudo make install

; libusb
sudo apt-get install libusb-1.0.0-dev

; curl
sudo apt-get install libcurl4-openssl-dev

Running using Valgrind:

make && valgrind --leak-check=full  --show-reachable=yes --show-leak-kinds=all --track-origins=yes ./skygrazer 
