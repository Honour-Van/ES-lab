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


