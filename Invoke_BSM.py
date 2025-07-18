import subprocess
import os
from itertools import product

# 调试信息
print(f"当前工作目录: {os.getcwd()}")

# 设置可执行文件路径（Windows格式）
EXECUTABLE_PATH = "BSM_Time.exe"

# 定义参数组合
n_values = [7107586,9028998,4087684,5506870,3943016,9987840,10000000,9999999]

# 定义三组参数组合
param_sets = [
    # 第一组：固定w=600000, k=20，变化s
    [{"w": 600000, "k": 20, "s": s} for s in [200, 100, 50, 20, 10]],
    
    # 第二组：固定k=20, s=50，变化w
    [{"w": w, "k": 20, "s": 50} for w in [200000, 400000, 600000, 800000, 1000000]],
    
    # 第三组：固定s=50, w=600000，变化k
    [{"k": k, "s": 50, "w": 600000} for k in [10, 20, 50, 100, 200]]
]

all_params = [p for subset in param_sets for p in subset]

total_runs = len(n_values) * len(all_params) * 5
current_run = 0

for n in n_values:
    for params in all_params:
        w, k, s = params["w"], params["k"], params["s"]
        
        for iteration in range(1, 6):
            current_run += 1
            print(f"执行 [{current_run}/{total_runs}]: n={n}, w={w}, k={k}, s={s}, 迭代={iteration}")
             
            cmd = [EXECUTABLE_PATH, str(n), str(w), str(s), str(k)]

            result = subprocess.run(
                    cmd,
                    capture_output=True,
                    text=True,
                )

print("所有任务执行完成！")