import random
import collections
data = 'bitch'

header = chr(0xAA)

to_send = header + data

zero_padding = '0' * 4
out_stream = zero_padding
for byte in to_send:
    bin_byte = bin(ord(byte)).replace('0b','').zfill(8)
    one_count = 1
    parity = '0'
    for bit in bin_byte:
        if bit == '1':
            one_count += 1
    one_count = one_count % 2
    if one_count == 1:
        parity = '1'
    out_byte = '0'+ bin_byte + parity + '1'
    print out_byte
    out_stream += out_byte
out_stream += zero_padding
print out_stream

stream = ''
for bit in out_stream:
    stream += bit*( 5 + random.randint(0,1) )

print stream

d = collections.deque( maxlen = 8 )

count = 1
bit_prev = ''
decoding = False
trans_count = 0
avg = 0
read = ''
decoded = ''
for bit in stream:
    if bit == bit_prev:
        count += 1
    if bit != bit_prev:
        if not decoding:
            trans_count += 1
            if trans_count > 8:
                avg = sum( d )/8
                if count > avg:
                    decoding = True
            d.append( count )
        else:
            bit_count = count / avg
            read += bit_prev * bit_count
            if len(read) >= 11:
                byte = read[0:11]
                parity = '0'

                one_count = 1
                for bit in byte[1:9]:
                    if bit == '1':
                        one_count += 1
                one_count = one_count % 2
                if one_count == 1:
                    parity = '1'

                # checks:
                # first bit = 0
                safe_byte = True
                if byte[0] != '0':
                    print 'WRONG START'
                    safe_byte = False
                if byte[10] != '1':
                    print 'WRONG END'
                    safe_byte = False
                if byte[9] != parity:
                    print 'WRONG PARITY'
                    safe_byte = False

                if safe_byte:
                    bits = byte[1:9]
                    print bits
                    char = chr( int(bits,2) )
                    decoded += char

                # last bit = 1
                # parity matches
                read = read[12:]
            #print bit_prev, count, avg, count / avg
        count = 1
    bit_prev = bit

print decoded
