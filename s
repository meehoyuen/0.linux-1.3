#!/bin/bash

if [ ! -e /etc/linux_init ];then
    ./cmt.sh
fi

if [ ! -e c.img ];then
    dd if=/dev/zero of=c.img count=163296
fi

#dd bs=1 if=c.img of=mbr.pt skip=446 count=48 2> /dev/null
make bzImage -s
if [ $? -ne 0 ];then
    exit 1
fi
dd if=arch/i386/boot/bzImage of=c.img conv=notrunc 2> /dev/null
dd bs=1 if=mbr.pt of=c.img seek=446 count=48 conv=notrunc 2> /dev/null

if [ $# -eq 0 ];then
    ./bochs -q
else
    bochs -q
fi
