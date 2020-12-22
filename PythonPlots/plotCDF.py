import numpy as np
import matplotlib.pyplot as plt


# Open files
avec = open("RESULT/benchmaks_Enclave_3.txt",'r')
sans = open("RESULT/benchmaks_NOEnclave_3.txt",'r')


# Convert to float

avecl = np.array(avec.readlines())
avecf = np.sort(np.array([float(item.strip()) for item in avecl[::2]]))

sansl = np.array(sans.readlines())
sansf = np.sort(np.array([float(item.strip()) for item in sansl[::2]]))


t = np.linspace(0.3,1.0,70)

X  = 0.3
dx = 0.01
CDF = [0]*70

for ele in sansf:
    if ele < X:
        p = int((X - 0.3)/dx)
        CDF[p]=CDF[p]+1
    else:
        X=X+dx
        p = int((X - 0.3)/dx)
        CDF[p]=CDF[p-1]

plt.plot(t,np.array(CDF)/sansf.size)
plt.hold(True)

X  = 0.3
CDF = [0]*70

for ele in avecf:
    if ele < X:
        p = int((X - 0.3)/dx)
        CDF[p]=CDF[p]+1
    else:
        X=X+dx
        p = int((X - 0.3)/dx)
        CDF[p]=CDF[p-1]

plt.plot(t,np.array(CDF)/avecf.size)


