import subprocess
import re
import random
from itertools import permutations
import random
from tqdm import tqdm

def main(prng=0):
    # 測試 shuffle 次數
    test_count = 100000
    input = "new\nit 1\nit 2\nit 3\nit 4\n"
    if prng:
        input += "option prng 1\n"
    input += "shuffle\n" * test_count
    input += "free\nquit\n"

    # 取得 stdout 的 shuffle 結果
    command='./qtest -v 3'
    clist = command.split()
    completedProcess = subprocess.run(clist, capture_output=True, text=True, input=input)
    s = completedProcess.stdout
    startIdx = s.find("l = [1 2 3 4]") 
    endIdx = s.find("l = NULL")
    s = s[startIdx + 14 : endIdx]
    Regex = re.compile(r'\d \d \d \d')
    result = Regex.findall(s)

    def permute(nums):
        nums=list(permutations(nums,len(nums)))
        return nums

    def chiSquared(observation, expectation):
        return ((observation - expectation) ** 2) / expectation 

    # shuffle 的所有結果   
    nums = []
    for i in result:
        nums.append(i.split())

    # 找出全部的排序可能
    counterSet = {}
    shuffle_array = ['1', '2', '3', '4']
    s = permute(shuffle_array)

    # 初始化 counterSet
    for i in range(len(s)):
        w = ''.join(s[i])
        counterSet[w] = 0

    # 計算每一種 shuffle 結果的數量
    for num in nums:
        permutation = ''.join(num)
        counterSet[permutation] += 1
    auto_corelation(nums, prng)
    # 計算 chiSquare sum
    expectation = test_count // len(s)
    c = counterSet.values()
    chiSquaredSum = 0
    for i in c:
        chiSquaredSum += chiSquared(i, expectation)
    print(f"PRNG: {"/dev/urandom" if prng == 0 else "xor shift"}")
    print("Expectation: ", expectation)
    print("Observation: ", counterSet)
    print("chi square sum: ", chiSquaredSum)
    import matplotlib.pyplot as plt
    import numpy as np

    # 假設 counterSet 是一個字典
    _permutations = counterSet.keys()
    counts = counterSet.values()

    x = np.arange(len(counts))
    plt.bar(x, counts)
    plt.xticks(x, _permutations, rotation=90)  # 避免 x 軸標籤重疊
    plt.xlabel('Permutations')
    plt.ylabel('Counts')
    plt.title(f"Shuffle Result ({'/dev/urandom' if prng == 0 else 'xor shift'})")

    # 存成圖片（可以指定格式，如 PNG、SVG、PDF 等）
    plt.savefig(f"shuffle_result({'dev_urandom' if prng == 0 else 'xor shift'}).png", dpi=300, bbox_inches='tight')
    # 關閉圖表，避免影響後續繪圖
    plt.close()    
def auto_corelation(input_data, prng):
    import numpy as np
    import math
    import matplotlib.pyplot as plt

    # 定義函數：根據字典順序計算排列的排名
    def permutation_rank(perm):
        """
        將排列 perm（例如 ['3', '1', '4', '2']）轉換成 0~23 的排名（字典順序）。
        """
        # 將元素轉成 int
        perm = list(map(int, perm))
        n = len(perm)
        rank = 0
        factorials = [math.factorial(i) for i in range(n+1)]
        # 取得初始可用數字（按照字典順序排序）
        available = sorted(perm)
        for i in range(n):
            index = available.index(perm[i])
            rank += index * factorials[n-i-1]
            available.pop(index)
        return rank

    # 假設 input_data 是你的輸入資料（每筆為一個排列，例如 ['1', '2', '3', '4']）
    # 範例：
    # input_data = [
    #     ['3', '1', '4', '2'],
    #     ['1', '2', '3', '4'],
    #     ... (其他資料) ...
    # ]

    # 將每筆排列編碼成 0~23 的整數
    encoded_data = np.array([permutation_rank(perm) for perm in input_data])
    data = encoded_data.astype(float)  # 轉換成 float 方便後續計算
    N = len(data)

    # 利用 numpy 計算自相關函數：
    # 1. 先將數據去除均值
    x = data - np.mean(data)
    # 2. 使用 np.correlate 計算 full 自相關 (模式 'full' 會產生長度 2N-1 的結果)
    auto_full = np.correlate(x, x, mode='full')
    # 3. 取非負延遲部分（從 index N-1 開始）
    acf = auto_full[N-1:]
    # 4. 正規化：令延遲為 0 時的值為 1
    acf = acf / acf[0]

    # 設定最大延遲期（例如 50）
    max_lag = 50
    lags = np.arange(max_lag+1)
    acf_vals = acf[:max_lag+1]

    # 繪製自相關函數圖
    plt.figure(figsize=(8, 4))
    plt.stem(lags, acf_vals, use_line_collection=True)
    plt.xlabel('Lag')
    plt.ylabel('Autocorrelation')
    plt.title(f"Autocorrelation Function({'/dev/urandom' if prng == 0 else 'xor shift'})")
    plt.savefig(f"ACF({'dev_urandom' if prng == 0 else 'xor shift'}).png", dpi=300, bbox_inches='tight')

    # 印出大約95%信賴區間（標準誤差約為 1/sqrt(N)）
    conf_level = 1.96 / np.sqrt(N)
    print(f"Approximate 95% confidence interval for autocorrelation: ±{conf_level:.4f}")

rng_cnt = 2
if __name__ == '__main__':
    for i in range(rng_cnt):
        print("start:")
        main(i)
        print()
