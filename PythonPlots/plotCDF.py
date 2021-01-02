import numpy as np
import matplotlib.pyplot as plt


### Ploting fuction ###


# Select file to plot
Files = ['Enclave_3','NOEnclave_3','RAW']

# Edit their legends
Ledg = ['SGX-Search','Algorithms','Raw queries']

# Set a title 
name  = "Cumulative distribution function of SGX\n reponsivness compare to algorithms alone (K = 3)"

# Parameters
start = 0.2
end = 0.6
n = 100

t = np.linspace(start,end,n)

## Job


for file in Files:
    f = open("RESULT/benchmaks_"+file+".txt",'r') 
    
    # Convert to float
    bmarks = np.array(f.readlines())
    bmarksFolat = np.sort(np.array([float(item.strip()) for item in bmarks[::2]]))


    X   = start
    dx  = (end-start)/n
    CDF = [0]*n
    p   = 0 
    
    # Compute CDF
    for ele in bmarksFolat:
        if ele < X:
            p = int((X - start)/dx)
            CDF[p]=CDF[p]+1
        else:
            while ele >= X and p+1<n-1:
                X=X+dx
                p = int((X - start)/dx)
                CDF[p]=CDF[p-1]
            CDF[p]=CDF[p]+1

        
    X=X+dx
    p = int((X - start)/dx)
    CDF[p]=CDF[p-1]    
    
    # Draw
    courbe = np.array(CDF)/bmarksFolat.size
    plt.plot(t,courbe)
    plt.hold(True)
    
    
## Plot Parameters

plt.legend(Ledg,loc='best', fontsize=14)
plt.xlabel('time (µs)')
plt.ylabel('% of queries under time t')
plt.title(name, fontsize=16)
plt.grid(1)

