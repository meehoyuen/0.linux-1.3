#!/bin/bash

if [ -e /etc/linux_init ];then
  exit 0
fi
sudo touch /etc/linux_init
sudo cp gawk /bin/

echo "set nu" > ~/.vimrc
echo "set tabstop=4" >> ~/.vimrc
echo "alias gg=\"grep -R --color=auto \"" >> ~/.bashrc
source ~/.bashrc
git config --global user.email "meeho@meeho"
git config --global user.name "meeho"
