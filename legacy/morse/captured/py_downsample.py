data = """
.======.......=======.....===....===......=.........===....==...=======.................==....===...=======...===........==...=======.......=======...===....=======...==........===...===...=======................====...===...=======...===.......===...=======.......=======....===..=======....===......====...===...=======................====...===...======
"""

dic = {
    "-----": "0",
    ".----": "1",
    "..---": "2",
    "...--": "3",
    "....-": "4",
    ".....": "5",
    "-....": "6",
    "--...": "7",
    "---..": "8",
    "----.": "9",
    ".-": "A",
    "-...": "B",
    "-.-.": "C",
    "-..": "D",
    ".": "E",
    "..-.": "F",
    "--.": "G",
    "....": "H",
    "..": "I",
    ".---": "J",
    "-.-": "K",
    ".-..": "L",
    "--": "M",
    "-.": "N",
    "---": "O",
    ".--.": "P",
    "--.-": "Q",
    ".-.": "R",
    "...": "S",
    "-": "T",
    "..-": "U",
    "...-": "V",
    ".--": "W",
    "-..-": "X",
    "-.--": "Y",
    "--..": "Z",
    "..--.-": "_",
    "-.-.--": "!",
    ".-..-.": '"',
    "...-..-": "$",
    ".-...": "&",
    ".----.": "'",
    "-.--.": "(",
    "-.--.-": ")",
    ".-.-.": "+",
    "--..--": ",",
    "-....-": "-",
    ".-.-.-": ".",
    "-..-.": "/",
    "---...": ":",
    "-.-.-.": ";",
    "-...-": "=",
    "..--..": "?",
    ".--.-.": "@"
        }

def tochar(code):
    if dic.has_key(code):
        return dic[code]
    return "?"

state_prev = ""
state_count = 0
dash_len = 0
stop_len = 0
code_buffer = ""
for state in data:
    if state == state_prev:
        if state == '=' and state_count > dash_len:
            dash_len = state_count
            stop_len = 2*dash_len
        state_count += 1
    else:
        if state_count > stop_len:
            # es un stop
            print code_buffer
            codes = code_buffer.split(' ')
            char_buffer = ""
            for code in codes:
                char_buffer += tochar( code )
            print char_buffer
            code_buffer = ""
        else:
            is_long = state_count >= dash_len
            if state_prev == '=':
                if is_long:
                    code_buffer += '-'
                else:
                    code_buffer += '.'
            else:
                if is_long:
                    code_buffer += ' '
        state_count = 1
    state_prev = state
