#data = open('lista.txt','rb').read()


state_prev = ""
state_count = 0

#print data

dash_len = 0
stop_len = 0
buffer = ""
out_code = ""

for state in open('lista.txt','rb').read():
    #print state
    if state == state_prev:
        state_count += 1
    else:
        if state_prev == '=':
            if state_count > dash_len:
                dash_len = state_count
                stop_len = dash_len * 2
            if state_count >= dash_len:
                out_code = '-'
            else:
                out_code = '.'
        else:
            if state_count <= dash_len:
                buffer += out_code
            if state_count >= dash_len:
                buffer += ' '
            if state_count >= stop_len:
                print dash_len, stop_len
                print buffer
                buffer = ""

        #print state_prev, state_count
        state_count = 1
    state_prev = state

"""
dash_len = 3
buffer = ''
prev_symbol = ''

out_code = ''
message = ''
for symbol in data:

    out_code = None

    if prev_symbol == symbol:
        buffer += symbol
    else:
        if len( buffer ) > 0:
            if buffer[0] == '=':#Dato alto

                # Corrijo la longitud del dato
                if len( buffer ) > dash_len:
                    dash_len = len( buffer )

                #print ( len( buffer ) / dash_len )
                if ( len( buffer ) / dash_len ) >= 1:
                    # es un dash
                    out_code = '='
                else:
                    out_code = '.'
            else:
                if ( len( buffer ) / dash_len ) > 1.8:
                    out_code = '\n'
                    buffer = ''
                elif ( len( buffer ) / dash_len ) >= 1:
                    out_code = ' '

        if out_code:
            buffer = ''
            message += out_code
                
    prev_symbol = symbol

print message
"""
