bs = "Whereas recognition of the inherent dignity and of the equal and inalienable rights"
print bs

blist = bs.split(' ')
for i in range(0, len(blist)):
    print blist[i]

count = 0
for loop in range (0,100000):
    blist = bs.split("of")
    count += len(blist)
print count
