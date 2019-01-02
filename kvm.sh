#!/bin/bash

which qemu-system-x86_64 > /dev/null
if [ $? -ne 0 ];then
    echo "sudo apt-get update"
    echo "sudo apt-get install qemu-kvm"
    exit 1
fi
qemu-system-x86_64 -smp 1 -m 64 -kernel arch/i386/boot/bzImage
