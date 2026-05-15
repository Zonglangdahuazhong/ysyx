# 1. 定义编译出来的程序名字（最后会生成一个叫 minirvEMU-native 的可执行文件）
NAME = minirvEMU

# 2. 包含你模拟器的所有 C 语言源文件（如果是多个文件，用空格隔开）
SRCS = minirvEMU.c

# 3. 核心：指引当前 Makefile 去寻找 AM 框架的总控制台
# 如果你的目录结构和我上面画的一样，用相对路径 ../../abstract-machine 即可
include $(AM_HOME)/Makefile
