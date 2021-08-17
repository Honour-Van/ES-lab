#!/bin/bash
./qemu/build/aarch64-softmmu/qemu-system-aarch64 -M virt -cpu cortex-a57 \
-nographic -smp 1 -m 2048 \
-kernel ./buildroot-edu/output/images/Image \
--append "rootwait root=/dev/vda console=ttyAMA0" \
-netdev user,id=eth0 -device virtio-net-device,netdev=eth0 \
-drive file=./buildroot-edu/output/images/rootfs.ext4,if=none,format=raw,id=hd0 \
-device virtio-blk-device,drive=hd0 \
-device edu