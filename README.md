# TopK
## 1. Dataset Section

| Filename       | Dataset Description                | Data Volume  | Additional Parameters                |
|----------------|-----------------------------------|--------------|---------------------------------------|
| A_total.txt    | Apple Stock Price Dataset         | 7,107,586    |                                       |
| B_total.txt    | Bitcoin Price Dataset             | 9,028,998    |                                       |
| C_total.txt    | Tesla Stock Price Dataset         | 4,087,684    |                                       |
| D_total.txt    | Amazon Stock Price Dataset        | 5,506,870    |                                       |
| E_total.txt    | Composite US Stock VTI.US Dataset | 3,943,016    |                                       |
| trip.txts      | New York Taxi Orders per 5 Minutes| 9,987,840    |                                       |
| normal.txt     | Generated Normal Distribution     | 10,000,000   | Mean=200, Standard Deviation=30       |
| uniform.txt    | Generated Uniform Distribution    | 10,000,000   | Max=100,000, Min=0                    |


## 2. Code Section

### (1) Execution Time Testing
- **C++ Source Files**:  
  `SkyCut_Time.cpp`, `ISM_Time.cpp`, `BSM_Time.cpp`, `RuzzoTompa_Time.cpp`, and `Tournament._Time.cpp` correspond to the code for five algorithms, used to test execution times under different datasets and parameter configurations.

- **Python Automation Scripts**:  
  These scripts automatically invoke the above C++ programs to perform five tests for each parameter setting across all datasets:  
  `Invoke_SkyCut.py`, `Invoke_ISM.py`, `Invoke_BSM.py`, `Invoke_RuzzoTompa.py`, `Invoke_Tournament.py`


### (2) Correctness Verification
For the more complex ISM and SkyCut algorithms, correctness verification scripts are provided. The verification process is as follows:  
1. Generate random test data using `DataGenerator`.  
2. Compute baseline results using the brute-force algorithm `Basic_Moving` (guaranteed correct).  
3. Compare the outputs of ISM/SkyCut with the baseline results to validate correctness.  

Run the following Python scripts to automatically verify correctness using 1,000 sets of random data:  
- `Invoke_TestResult_ISM.py` (Verify ISM algorithm)  
- `Invoke_TestResult_SkyCut.py` (Verify SkyCut algorithm)
