1.数据部分：
A_total.txt是苹果股价数据集，数据量为7107586
B_total.txt是比特币价格数据集，数据量为9028998
C_total.txt是特斯拉股价数据集，数据量为4087684
D_total.txt是亚马逊股价数据集，数据量为5506870
E_total.txt是组合美股VTI.US股价数据集，数据量为3943016
trip.txts是纽约每五分钟内出租车订单量数据集，数据量为9987840
normal.txt是生成的正态分布数据集，数据量为10000000，均值为200，标准差为30
uniform.txt是生成的均匀分布数据集，数据量为10000000，最大值为100000，最小值为0

2.代码部分：
(1)运行时间测试：
SkyCut_Time.cpp、ISM_Time.cpp、BSM_Time.cpp、RuzzoTompa_Time.cpp、Tournament._Time.cpp分别对应五个方法的代码，用于测试代码在不同数据集、不同参数设置下的运行时间。
使用五个python脚本自动调用上述五个程序，对所有的数据集的每一个参数设置情况，进行五次测试，五个脚本分别为Invoke_SkyCut.py、Invoke_ISM.py、Invoke_BSM.py、Invoke_RuzzoTompa.py、Invoke_Tournament.py

(2)正确性检验：
对于较为复杂的ISM和SkyCut算法，提供正确性检验的脚本。
首先使用DataGenerator生成随机数据，使用一个有正确性保证的暴力算法Basic_Moving生成标准正确结果，再将其与ISM和SkyCut的输出结果进行对比，即可验证其正确性。
具体地，运行python脚本Invoke_TestResult_ISM.py和Invoke_TestResult_SkyCut.py，会使用1000组随机数据，对ISM和SkyCut的正确性进行自动验证。

