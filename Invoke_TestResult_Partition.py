import subprocess  
from collections import namedtuple 

def count_lines(file):  
        file.seek(0)
        return sum(1 for line in file)  # 通过生成器计算行数  

def compare_files(output_BF,output_Basic):
    with open(output_BF, 'r') as f1, open(output_Basic, 'r') as f2:
        cnt_tot_BF=count_lines(f1)
        cnt_tot_Basic=count_lines(f2)
        f1.seek(0)
        f2.seek(0)
        cnt_wrong=0
        for line1,line2 in zip(f1,f2):
#            print("line1"+line1)
#            print("line2="+line2)
            if line1 != line2:
                cnt_wrong += 1

        Result = namedtuple('Result', ['cnt_BF','cnt_Basic','cnt_wrong'])  
        return Result(cnt_tot_BF,cnt_tot_Basic,cnt_wrong)


n=2000
w=1000
s=10
k=20

delta = 0
correct_ans = 0
for _ in range(1000):
    # 调用 C++ 程序  
    result_Data = subprocess.run(['./DataGenerator.exe',str(n),str(w)], capture_output=True, text=True)  
    # 输出结果 

    result_BF = subprocess.run(['./Basic_Moving.exe',str(n),str(w),str(s),str(k)], capture_output=True, text=True) 

    result_Basic = subprocess.run(['./TestResult_Partition.exe',str(n),str(w),str(s),str(k)], capture_output=True, text=True) 



    #数据生成器将数据填入input中
    #暴力代码读取input中数据并运行，将数据填入output_BF中
    #Basic方法读取input中数据并运行，将数据填入output_Basic中

    result = compare_files('Basic_Moving.txt','Partition.txt')
    correct_ans+=1
    if(result.cnt_BF != result.cnt_Basic):
        print("Wrong!!????????????????")
        correct_ans-=1
        break
    if(result.cnt_wrong!=0):
        print("Wrong!!????????????????")
        correct_ans-=1
        break

    delta+=1
    if(delta==50): 
        print(correct_ans)
        delta=0

print("correct_ans=" + str(correct_ans))
    
    


#比对函数读取output_BF和output_Basic，进行对比

