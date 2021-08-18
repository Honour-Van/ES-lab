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



按照已有的一些资料的说法（比如https://zhuanlan.zhihu.com/p/350947593），我们首先将default-config文件中的CONFIG_EDU打开。

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

失败，配置并不是这么简单的。

> 事后思考：这里忽略了两个要点：
> 1. 没有考虑到不同板子之间的差异
> 2. 强行寻找一个qemu的配置文件

**一些思考**：

在完成

1. 料想问题出现在没有对PCI设备这个性质进行足够的关注：
   1. https://qemu.readthedocs.io/en/latest/system/device-emulation.html?highlight=pci
   2. https://qemu.readthedocs.io/en/latest/system/target-arm.html?highlight=pci
2. 不应该拘泥于已有的教程，因为重构之后，不能确定这个*不太重要*的edu在哪里。最新版本中，整个repo只有一个与edu相关的代码文件，这已经是PCI的驱动程序了，我们所需做的，是仿照misc平台下的这个驱动在arm vexpress/versatilepd中实现。![image-20210816211400313](assets/image-20210816211400313.png)
3. 仍应该寻找一个相对好的教程先熟悉自定义device的问题：https://milokim.gitbooks.io/lbb/content/qemu-how-to-design-a-prototype-device.html




## 向virt板中自定义添加device
按照milokim给出的示例先复现：

安装qemu时出错：https://blog.csdn.net/qq_36393978/article/details/118086216


## 了解PCI device添加方式

暂时可以参考的是下面这一篇：https://github.com/levex/kernel-qemu-pci

另外直接找到添加edu的方法，注意文件结构已经改过，在qemu/configs/devices/aarch64-softmmu/default.mk中添加CONFIG_EDU=y，随后重新编译

## PCI驱动相关

了解lspci和setpci等常见pci utils


## 一些杂项

linux下的查找命令效率很高，比如查找hw为名的目录，其中和qemu相关的部分如下：

```shell
$ find . -name hw -type d
./output/build/host-qemu-6.0.0/include/hw
./output/build/host-qemu-6.0.0/include/standard-headers/drivers/infiniband/hw
./output/build/host-qemu-6.0.0/roms/skiboot/hw
./output/build/host-qemu-6.0.0/roms/seabios/src/hw
./output/build/host-qemu-6.0.0/roms/seabios-hppa/src/hw
./output/build/host-qemu-6.0.0/hw
./output/build/host-qemu-6.0.0/build/hw
```



8月18日方案：重新按照**aarch64**对环境进行重新配置。
1. 安装buildroot并按照arm64-virt进行硬件配置
2. 下载qemu，并在aarch64环境下进行configure+make
3. 寻找在arm64平台下安装驱动的方法，安装aarch64-linux-gnu-gcc-10并重命名


$ git clone git://git.qemu.org/qemu.git
$ cd qemu
$ ./configure --target-list=aarch64-softmmu
$ make

git clone git@github.com:buildroot/buildroot.git
$ cd buildroot
$ make qemu_aarch64_virt_defconfig
$ make


将内核驱动放入文件系统可以在两个阶段完成，分别是buildroot make之前和之后，之前就对应着向buildroot package中添加相关信息，随后一次烧制，方法如下：
https://buildroot.org/downloads/manual/manual.html#_infrastructure_for_packages_building_kernel_modules

也可以使用更加方便的交叉编译方法。

运行脚本./run_aarch64.sh

```shell
# lspci
00:01.0 Class 00ff: 1234:11e8
00:00.0 Class 0600: 1b36:0008
# insmod hello.ko 
 book name:dissecting Linux Device Driver
 book num:4000
```


```c
# insmod edu.ko 
lkmc_pci 0000:00:01.0: pci_probe
lkmc_pci 0000:00:01.0: enabling device (0000 -> 0002)
EDU device version: 10000ed
length 100000
config 0 34
config 1 12
config 2 e8
config 3 11
config 4 2
config 5 0
config 6 10
config 7 0
config 8 10
config 9 0
config a ff
config b 0
config c 0
config d 0
config e 0
config f 0
config 10 0
config 11 0
config 12 0
config 13 10
config 14 0
config 15 0
config 16 0
config 17 0
config 18 0
config 19 0
config 1a 0
config 1b 0
config 1c 0
config 1d 0
config 1e 0
config 1f 0
config 20 0
config 21 0
config 22 0
config 23 0
config 24 0
config 25 0
config 26 0
config 27 0
config 28 0
config 29 0
config 2a 0
config 2b 0
config 2c f4
config 2d 1a
config 2e 0
config 2f 11
config 30 0
config 31 0
config 32 0
config 33 0
config 34 40
config 35 0
config 36 0
config 37 0
config 38 0
config 39 0
config 3a 0
config 3b 0
config 3c 31
config 3d 1
config 3e 0
config 3f 0
dev->irq 31
io 0 10000ed
io 4 0
io 8 0
io c ffffffff
io 10 ffffffff
io 14 ffffffff
io 18 ffffffff
io 1c ffffffff
io 20 0
io 24 0
lkmc_pci 0000:00:01.0: vaddr_from = ffffff804168e000
lkmc_pci 0000:00:01.0: dma_handle_from = 8168e000
lkmc_pci 0000:00:01.0: vaddr_to = ffffff804166c000
lkmc_pci 0000:00:01.0: dma_handle_to = 8166c000
# EDU: clamping DMA 0x000000008168e000 to 0x000000000168e000!
irq_handler irq = 49 dev = 250 irq_status = 100
```

注意，mmio的前80个字节只能以4 byte为单位读入，比如读取版本号时，不能读取逐个byte读取。
```c
pr_info("EDU device version: %x.%x\n", ioread8((void*)(mmio)), ioread8((void*)(mmio+1)));
```

否则会报错：
```shell
# insmod edu.ko 
lkmc_pci 0000:00:01.0: pci_probe
lkmc_pci 0000:00:01.0: enabling device (0000 -> 0002)
Internal error: synchronous external abort: 96000010 [#1] SMP
Modules linked in: edu(+)
CPU: 0 PID: 101 Comm: insmod Not tainted 5.10.7 #1
Hardware name: linux,dummy-virt (DT)
pstate: 60000005 (nZCv daif -PAN -UAO -TCO BTYPE=--)
pc : __raw_readb+0x4/0x10 [edu]
lr : pci_probe+0x160/0xc28 [edu]
sp : ffffffc010cbb910
x29: ffffffc010cbb910 x28: 0000000000000013 
x27: 0000000000000100 x26: ffffffc008692280 
x25: ffffffc0100d4aa0 x24: 0000000000000002 
x23: ffffffc008692160 x22: 0000000000000000 
x21: ffffffc008692580 x20: ffffff80403c58b0 
x19: ffffff80403c5800 x18: 0000000000000020 
x17: 0000000000000000 x16: 000000000000000b 
x15: ffffffffffffffff x14: ffffffc090cbb84d 
x13: ffff000000000000 x12: 0000000000000000 
x11: 0101010101010101 x10: ffffffff7f7f7f7f 
x9 : 0000000000000000 x8 : ffffff8041615780 
x7 : 0000000040000000 x6 : 0000000000000004 
x5 : ffffff8041615508 x4 : ffffff80416155c8 
x3 : 0000000000000000 x2 : 0000000000000000 
x1 : 0000000000000000 x0 : ffffffc010d00000 
Call trace:
 __raw_readb+0x4/0x10 [edu]
 pci_device_probe+0xb8/0x150
 really_probe+0xd4/0x490
 driver_probe_device+0x58/0xc0
 device_driver_attach+0xc0/0xd0
 __driver_attach+0x84/0x130
 bus_for_each_dev+0x70/0xd0
 driver_attach+0x24/0x30
 bus_add_driver+0x104/0x1f0
 driver_register+0x78/0x130
 __pci_register_driver+0x48/0x60
 myinit+0x2c/0x3c [edu]
 do_one_initcall+0x50/0x1b0
 do_init_module+0x54/0x250
 load_module+0x1e78/0x24c0
 __do_sys_finit_module+0xb8/0x100
 __arm64_sys_finit_module+0x24/0x30
 el0_svc_common.constprop.0+0x94/0x1c0
 do_el0_svc+0x40/0xb0
 el0_svc+0x14/0x20
 el0_sync_handler+0x1a4/0x1b0
 el0_sync+0x174/0x180
Code: 88dffc84 d500409f d500419f d503233f (08dffc00) 
---[ end trace 7b668654bc19c325 ]---
Segmentation fault
```
