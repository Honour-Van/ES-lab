## lab3 编写 Linux 驱动程序
要求：
1. 为 qemu 中的 edu 设备编写 PCI 设备驱动程序，基本要求是读出设备的版本号即可。
2. 如果对更多的驱动功能感兴趣也可以支持更多的功能（中断/DMA等）（edu 设备的 ARM 版我没有测试过，也可能行不通）
            
1'. （简化版）编写一个可以存储少量数据的驱动，通过字符设备接口进行读写。

报告要求：
1. 附上驱动程序源码和测试程序代码（如果有）
2. 简要说明遇到的问题和解决方法

参考资料：
1. qemu 的源代码，主要看看里面的 docs/specs/edu.txt
2. Linux Device Driver 第三版
3. 网络资源

```shell
sudo find ~/ -name default-configs -type d
/home/fhn/eslab/buildroot/output/build/host-qemu-6.0.0/default-configs
```


```shell
qemu-system-arm \
-M versatilepb \
-kernel zImage \
-dtb versatile-pb.dtb \
-drive file=rootfs.ext2,if=scsi,format=raw \
-append "rootwait root=/dev/sda console=ttyAMA0,115200"  \
-net nic,model=rtl8139 \
-net user \

```

失败。



料想问题出现在没有对PCI设备这个性质进行足够的关注。

https://qemu.readthedocs.io/en/latest/system/device-emulation.html?highlight=pci
