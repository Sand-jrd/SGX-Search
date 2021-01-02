import numpy as np
import matplotlib.pyplot as plt

import numpy as np
import matplotlib.pyplot as plt


### Ploting fuction ###


# Select file to plot
K = [2,3,4,5]
reference = 'RAW'


# Edit their legends
Ledg = ['SGX-Search K = 2','SGX-Search K = 3']

# Set a title 
name  = "My figure"

# Parameters
n    = 4500
maxr = 15
metric = []

## Job


# Process reference

ref = open("RESULT/output_"+reference+".txt",'r') # Read from file
outputsR = np.array(ref.readlines())

reqREF = []
for input in range(0,len(outputsR)-1):            # Store proprely
    sub = outputsR[input].split("\t")
    sub.remove('\n')
    if(len(sub)>0):
        reqREF.append(sub)
            
            
# Do job

for file in K:
    
    f = open("RESULT/output_Enclave_"+str(file)+".txt",'r')   # Read from file
    outputs = np.array(f.readlines())

    req = []
    for input in range(0,len(outputs)-1):        # Store proprely
        sub = outputs[input].split("\t")
        sub.remove('\n')
        if(len(sub)>0):
            req.append(sub)

    # Count common
    countCommon = []
    for input in range(1,n):                     # Count common links
        intersection = set(reqREF[input]) & set(req[input])
        countCommon.append((len(intersection))/len(req[input]))
        

   # plot metrics as histogram bar
    plt.hist([file-0.15],1, weights=[(sum(countCommon)/len(countCommon))],color = ['green'],
            edgecolor = 'black',rwidth = 0.3)
    plt.hist([file+0.15],1, weights=[(sum(countCommon)/len(countCommon))],color = ['yellow'],
            edgecolor = 'black',rwidth = 0.3)
    plt.hold(True)



## Plot Parameters


plt.title(name)
plt.xticks(np.arange(2,6), ["K=2","K=3","K=4","K=5"], rotation=20)
plt.ylabel('%')
plt.grid(1)

# Some trick for setting proprely the legend
plt.hist([file-0.15],1, weights=[(sum(countCommon)/len(countCommon))],color = ['green'],
        edgecolor = 'black',rwidth = 0.3, label = ['Recall'])
plt.hist([file+0.15],1, weights=[(sum(countCommon)/len(countCommon))],color = ['yellow'],
        edgecolor = 'black',rwidth = 0.3, label = ['Precision'])
plt.legend(loc='best')