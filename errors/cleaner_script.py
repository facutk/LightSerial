lines = open("rwmorse", "rb").readlines()
parsed = []
for line in lines:
    parsed.append( bin(int(line,16)).replace('0b','').zfill(32)[16:] )

fout = open("morse_limpio.txt", "wb")
fout.write( "\n".join(parsed) )
fout.close()
