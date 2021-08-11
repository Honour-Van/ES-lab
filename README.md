# ES-lab
labs for embedded system, 2021 summer

## lab1 在 qemu 中运行自己的文件系统和内核
要求：
1. 内核与文件系统都使用源代码编译生成二进制目标文件，通过 qemu 测试运行自己的内核与文件系统。
2. 使用 buildroot 即可完成，体系结构推荐使用 arm（具体开发板类型不限），也可以使用 x86（相对简单些）生成 Linux 操作系统的内核和可用的文件系统，用 qemu 进行测试。

报告要求：
1. 说明自己使用的源代码版本，基本操作过程，和最终成品截图。
2. 截图要使用 uname -a 命令显示所使用的内核版本号。
3. 在操作过程中遇到的主要问题和解决方法是什么。

参考资料：网络

## lab2 编写 socket 通信程序，实现简单交互聊天功能。
要求：服务器端在主机运行，客户端在 qemu 模拟器中运行，能实现交互聊天即可。

报告要求：全部源代码，运行界面截图。

## lab3 编写 Linux 驱动程序
要求：
1. 为 qemu 中的 edu 设备编写 PCI 设备驱动程序，基本要求是读出设备的版本号即可。
2. 如果对更多的驱动功能感兴趣也可以支持更多的功能（中断/DMA等）（edu 设备的 ARM 版我没有测试过，也可能行不通）
      1'. （简化版）编写一个可以存储少量数据的驱动，通过字符设备接口进行
         读写。
报告要求：
1. 附上驱动程序源码和测试程序代码（如果有）
2. 简要说明遇到的问题和解决方法

参考资料：
1. qemu 的源代码，主要看看里面的 docs/specs/edu.txt
2. Linux Device Driver 第三版
3. 网络资源

## lab4 编写 QT 图形界面程序
要求：
1. 五子棋对抗小游戏，需要有图形界面，用户交互。
2. 具有一定网络功能。可以作为服务器等待其它用户接入或者接入其它服务器。

通信协议如下：

服务器应有功能：规则判断，棋局记录，用时计算。

示例协议（-> 表示发送，没有 -> 表示接收，在客户端接入之后发送NG）

- ->NG    ; 表示新游戏
- OK      ; 正常
- ->MV h8 ; h8位置下棋
- MV h9   ; 对手下棋
- ->MV g7
- ER 1    ; 错误1：禁手，错误2：位置不空 ...
- ->MV d5
- OV 1    ; 1 表示胜利，0表示失败
- OV T    ; 表示对方超时（选做）。

报告要求：
1. 附上全部代码
2. 简要对代码进行说明

参考资料：网络与 QT 在线文档
