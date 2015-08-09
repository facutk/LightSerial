import collections

codes = {
    '0000': '11110',
    '0001': '01001',
    '0010': '10100',
    '0011': '10101',
    '0100': '01010',
    '0101': '01011',
    '0110': '01110',
    '0111': '01111',
    '1000': '10010',
    '1001': '10011',
    '1010': '10110',
    '1011': '10111',
    '1100': '11010',
    '1101': '11011',
    '1110': '11100',
    '1111': '11101'
}

zero_pad = ''#'0' * 4
preamble = '' #'11111111110'
sep = ''
data = chr(0b11100100) #'o'
print bin(ord(data))
_4b5b = ''
_4b5b += zero_pad + preamble + sep

for byte in data:
    tmp = bin(ord(byte)).replace('0b','').zfill(8)
    _4b5b += ''.join( [ codes[tmp[i: i + 4]] for i in range(2)] )
    _4b5b += sep 
#_4b5b = _4b5b.replace(sep, '\n')
print _4b5b

nrzi = ''
sync = 0
for code in _4b5b:
    if code == '1':
        sync = 1 - sync

    if sync == 1:
        code = '1'
    else:
        code = '0'

    nrzi += code
print nrzi

physical = ''
for code in nrzi:
    code = code * 3
    physical += code

print physical

#decoding
decoded = ''
reading_prev = '0'
count = 0
sync = 0
decoding = False
trans_count = 0
d = collections.deque( maxlen = 8 )
for reading in physical:
    if reading == reading_prev:
        count +=1
    else:
        if not decoding:
            trans_count += 1
            d.append( count )
            if trans_count > 8 and count > sum(d)/8:
                decoding = True

        if decoding:
            print count
        sync = 1 - sync
        if sync == 1:
            code = '1'
        else:
            code = '0'

        decoded += code
        count = 1
    reading_prev = reading

print decoded
