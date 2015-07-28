# R W C T X
# 0 0 0 0 0
# - - - - -
# 0 0 1
# 0   2
# 0   3
# 0   4
# 1     1
# 0   6
# 0   7
# 1     1
# 1     2
# 1 1 3   7 -> 0(7)
# 0     1
# 0     2
# 1   6
# 1   7
# 1   8
# 0     1
# 0     2
# 0 0 3   8 -> 1(8)

# R --n--> |W, C, T| --k--> W,C

working = '0'
count = 0
transition = 0
threshold = 2

stream = "000010011100111000"
for reading in stream:
    if reading == working:
        count += 1
        if transition:
            count += transition
            transition = 0
    else:
        transition += 1
        if transition > threshold:
            print working, count # --> Called once per transition
            count = transition
            transition = 0
            working = reading
