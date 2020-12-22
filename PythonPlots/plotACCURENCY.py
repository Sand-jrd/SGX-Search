import numpy as np
import matplotlib.pyplot as plt


n    = 2000
maxr = 15

# Open files
a4 = open("RESULT/output_Enclave_3.txt",'r')
a5 = open("RESULT/output_Enclave_4.txt",'r')


# Convert to float

a4l = np.array(a4.readlines())
a5l = np.array(a5.readlines())

sub = []
req4 = []
for input in range(1,len(a4l)):
    if a4l[input] != '\n':
        sub.append(a4l[input])
    else:
        if(len(sub)>0):
            req4.append(sub[0].split("\t"))
            sub = []
    
sub = []
req5 = []
for input in range(1,len(a5l)):
    if a5l[input] != '\n':
        sub.append(a5l[input])
    else:
        if(len(sub)>0):
            req5.append(sub[0].split("\t"))
            sub = []
    

countCommon = []
for input in range(1,n):
    intersection = set(req4[input]) & set(req5[input])
    countCommon.append(len(intersection)-1)
    
plt.plot(countCommon)