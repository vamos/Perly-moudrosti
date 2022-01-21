#!/bin/bash

echo $1

pushd $1

cat Readme* readme* README* Readme.* readme.* README.* | more

echo -e "\e[7m" && read -p "                  Press enter to continue                  " && echo -e "\e[0m"

echo -e "\e[7m                  Test1 - f/f                  \e[0m"
./isa-tazatel -q nato.int -w whois.ripe.net
echo -e "\e[7m" && read -p "                  Press enter to continue                  " && echo -e "\e[0m"

echo -e "\e[7m                  Test2 - f/v4                 \e[0m"
./isa-tazatel -q vlada.cz -w 217.31.205.42
echo -e "\e[7m" && read -p "                  Press enter to continue                  " && echo -e "\e[0m"

echo -e "\e[7m                  Test3 - v4/v4                  \e[0m"
./isa-tazatel -q 147.229.2.90 -w 193.0.6.135
echo -e "\e[7m" && read -p "                  Press enter to continue                  " && echo -e "\e[0m"

echo -e "\e[7m                  Test4 - f/v6                  \e[0m"
./isa-tazatel -q seznam.cz -w 2001:1488:0:2::2
read -p "Press enter to continue"

echo -e "\e[7m                  Test5 - v4/v6                 \e[0m"
./isa-tazatel -q 77.75.75.172 -w 2001:67c:2e8:22::c100:687
read -p "Press enter to continue"

echo -e "\e[7m                  Test6 - v6/v4                 \e[0m"
./isa-tazatel -q 2001:718:1e03:622::12 -w 193.0.6.135
read -p "Press enter to continue"

echo -e "\e[7m                  Test7 - v6/v6                  \e[0m"
./isa-tazatel -q 2001:718:1e03:622::12 -w 2001:67c:2e8:22::c100:687
read -p "Press enter to continue"

echo -e "\e[7m                  Test8                  \e[0m"
./isa-tazatel -q ...fit.vutbr.cz... -w whois.ripe.net
echo -e "\e[7m" && read -p "                  Press enter to continue                  " && echo -e "\e[0m"

echo -e "\e[7m                  Test9                  \e[0m"
./isa-tazatel -q 147.229.2.900 -w whois.ripe.net
echo -e "\e[7m" && read -p "                  Press enter to continue                  " && echo -e "\e[0m"

echo -e "\e[7m                  Test10                  \e[0m"
./isa-tazatel -q 2001:67c:1220:809::93e5:917X -w whois.ripe.net
echo -e "\e[7m" && read -p "                  Press enter to continue                  " && echo -e "\e[0m"

echo -e "\e[7m                  Test11                  \e[0m"
./isa-tazatel -q nix.cz -w whois.nic.cz -d 1.1.1.1
echo -e "\e[7m" && read -p "                  Press enter to continue                  " && echo -e "\e[0m"

popd
