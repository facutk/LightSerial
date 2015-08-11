def emit( char ):
    if char:
        emit.msg += char
    else:
        print emit.msg
        emit.msg = ""
emit.msg = ""

def decode( signal, count ):
    if signal == '1':
        if count > decode.dash_len:
            decode.dash_len = count
            decode.dot_len = decode.dash_len / 2
            decode.stop_len = 2*decode.dash_len
        decode.index *= 2
        if count < decode.dot_len:
            decode.index += 1
        else:
            decode.index += 2
    else:
        if count > decode.dot_len:
            if decode.index <= len( decode.lookup ):
                char = decode.lookup[ decode.index ]
                emit( char )
            decode.index = 0
            if count > decode.stop_len: # message ready
                emit( 0 )
                #decode.dot_len  = 0
                #decode.dash_len = 0
                #decode.stop_len = 0
decode.lookup = " ETIANMSURWDKGOHVF L@PJBXCYZQ-*54.3,:|2!?+#$&'16=/()<>[7]{}8_90"
decode.index = 0
decode.dot_len = 40#40
decode.dash_len = 40#40
decode.stop_len = 40#40

def denoise( reading ):
    if reading == denoise.working:
        if denoise.count < 255:
            denoise.count += 1
        if denoise.transition:
            denoise.count += denoise.transition
            denoise.transition = 0
    else:
        denoise.transition += 1
        if denoise.transition > denoise.threshold:
            decode( denoise.working, denoise.count )
            denoise.count = denoise.transition
            denoise.transition = 0
            denoise.working = reading
denoise.working = '0'
denoise.count = 0
denoise.transition = 0
denoise.threshold = 2
#denoise.timedout = False

if __name__ == "__main__":
    file = "morse_limpio.txt"

    stream = "".join(open(file, "rb").readlines())
    stream = stream.replace("\n","")

    for reading in stream:
        denoise( reading )