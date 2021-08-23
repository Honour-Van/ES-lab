# lab4 编写 QT 图形界面程序

lab4 编写 QT 图形界面程序

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


## 构思

思考整体由网络和图形界面两部分组成。图形界面提供用户交互的内容，主要包括开始游戏的选择，游戏房间的建立，下棋位置信息的记录，合法性判断，局面判断。

另外按照上述要求中的示例，构建一个网络模块原型`socket.cpp`，集成server和client两种接口。
1. server：创建房间，要求读出当前机器的IP
2. client：加入房间，要求读入服务器端的IP

经过测试意识到，可以直接使用Qt的socket组件。

我们在github的一个五子棋开源项目https://github.com/tashaxing/QtWuziqi 的基础上，添加socket通信相关组件，实现联机对战模式。

在server端记录棋盘（或许可以在client端同步，这样判别时效率会更高）。

开始游戏，房间建立，棋盘数据，以及暂停等等，都交由Qt来提供。

游戏开始之后：
1. 建立连接
2. 选择颜色
   1. 看谁先走，然后另一个人就选择另一种颜色
   2. 由于打印棋子颜色是交替进行的，所以只需做初始化
3. 每次走完（基于mouseReleaseEvent，因而可以按住鼠标的时间内修改棋步）将自己的棋步发送出去，然后等待接收
4. 接收完之后显示


结合
- https://www.cnblogs.com/findumars/p/5838531.html
- https://blog.csdn.net/rl529014/article/details/52884845
- https://blog.csdn.net/u012234115/article/details/53871009
- https://github.com/tashaxing/QtWuziqi

实现过程中遇到的问题：
1. server可以接收到，client不能。（考虑到我们将client和server集成在一起，按照实际情景实例化，不太熟悉的API和分支一起使用时，要看看这些分支的逻辑是否完备。多加debug语句，有助于我们理解陌生语言和环境）
2. 由于直接加入了mouseReleaseEvent中，如果某次没有点到有效位置，也会产生sendMessage调用，在接下来的位置传输时要注意。
3. 源码中给出的实现思路无法重新开盘
   1. 本来以为和二维vector的清除有关，如这篇文章中所述https://blog.csdn.net/stephen___qin/article/details/71024160
   2. 猜想是segmentation fault，但是由于qt的多线程循环的结构，调试起来非常不方便。并没有找到错误所在。
   3. debug不能正常启动的原因猜测和代码本身无关。segFault出现在系统库文件中：
   ```cpp
      0x7ffadfb468f0                  cc                       int3
   -> 0x7ffadfb468f1  <+    1>        c3                       retq
      0x7ffadfb468f2  <+    2>        cc                       int3
      0x7ffadfb468f3  <+    3>        cc                       int3
      0x7ffadfb468f4  <+    4>        cc                       int3
      0x7ffadfb468f5  <+    5>        cc                       int3
      0x7ffadfb468f6  <+    6>        cc                       int3
      0x7ffadfb468f7  <+    7>        cc                       int3
      0x7ffadfb468f8  <+    8>        0f 1f 84 00 00 00 00 00  nopl   0x0(%rax,%rax,1)
   ```
   怀疑同版本有关。
4. 资源的prefix管理有一点问题。做了一定的调整，并增加了棋盘背景、棋子等资源。
5. 添加了图片：
   ```cpp
    QPixmap pix;   
    pix.load(":/res/board1.jpg");
    painter->drawPixmap(kBoardMargin, kBoardMargin, kBlockSize * kBoardSizeNum, kBlockSize * kBoardSizeNum, pix);
   ```
6. 添加状态菜单，包括
   1. 文字的使用和自定义https://blog.csdn.net/zhizhengguan/article/details/113384032
   2. 时间变量的使用
   3. Qt多线程和connect-signal-slot机制：https://blog.csdn.net/t46414704152abc/article/details/52133377 https://blog.csdn.net/humanking7/article/details/86071134
7. 