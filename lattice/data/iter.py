

def write_result(R, fname):
    fout=open(fname, 'w')
    label = '\t'.join(map(str, R[R.keys()[0]].keys()))
    fout.write('\t'.join(['0.00', label]) + '\n')
    for p in R:
        value = '\t'.join(map(str, R[p].values()))
        fout.write('\t'.join([str(p), value]) + '\n')
    fout.close()
    
T = {}

for line in open('result.dat'):
    n, c, p, it, _ = map(float, line.rstrip().split('\t'))
    T.setdefault(p, {})
    T[p].setdefault(c, [])
    T[p][c].append(it)

TT = {}
for p in T:
    TT[p] = {}
    for c in T[p]:
        TT[p][c] = sum(T[p][c])/float(len(T[p][c]))

###variance
##        
##TT = {}
##for p in T:
##    TT[p] = {}
##    for c in T[p]:
##        mean = sum(T[p][c])/float(len(T[p][c]))
##        TT[p][c] = sum([(x-mean)**2 for x in T[p][c]])

        
write_result(TT, 'mean.dat')
