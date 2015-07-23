lines = open("real_world_data", "rb").readlines()
parsed = []
for line in lines:
    parsed.append( bin(int(line,16)).replace('0b','').zfill(32)[16:] )

fout = open("limpio.txt", "wb")
fout.write( "\n".join(parsed) )
fout.close()
