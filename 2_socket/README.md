# lab2 编写 socket 通信程序，实现简单交互聊天功能

要求：
1. 能实现交互聊天即可。
2. 服务器端在主机运行，客户端在 qemu 模拟器中运行。

## 项目分析

这个项目分为两个大的部分：
1. socket编程实现聊天程序，基于经典的C/S结构分别编写server端和client端程序server.c和client.c。
2. 向buildroot中添加自定义程序。

实际上，由于第一部分相对偏应用，能找到比较好的开源代码以借鉴参考，问题的难点反而在于第二部分。

关于有两种实现思路：
1. 在buildroot中添加vi和gcc等，然后在运行时现场编写运行。
2. 在buildroot的menuconfig阶段就将相应的配置文件、源代码和Makefile编写好，然后重新make，将二进制的可运行文件写入文件系统中。

虽然第二种相对陌生，但由于buildroot有现成的tutorial，探索的成本也并不高。这种方式更接近硬件编程的思路，所以这里首先对第二种进行尝试。

## 项目实现

### socket编程

我们为了快速迭代，首先采用了https://www.cnblogs.com/liushao/p/6375377.html 中给出的示例代码。并在本地进行了测试。

进行第三方包的安装尝试：

参考https://www.cnblogs.com/arnoldlu/p/9553995.html的第五部分。

注意Config.in的编写利用menu和endmenu进行闭包和配对。

![image-20210813223850227](assets/image-20210813223850227.png)


随后需要保存设置。
![image-20210813223741555](assets/image-20210813223741555.png)

最终成功

![image-20210814114758774](assets/image-20210814114758774.png)

![image-20210814130550821](assets/image-20210814130550821.png)


https://www.cnblogs.com/liushao/p/6375377.html
