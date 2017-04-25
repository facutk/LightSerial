/*
* morseMessageTiming
* 
* usage:
*   morseMessage(message, minDelay, callbal);
*
* example:
*   import morseMessageTiming from './morseMessageTiming';
*   morseMessageTiming('morse', 80, (action) => {console.log(action);});
*/

const lookup = "rETIANMSURWDKGOHVF L@PJBXCYZQ-*54.3,:|2!?+#$&'16=/()<>[7]{}8_90";

const decode = (code) => {
  let index = 0;
  const code_len = code.length;
  let i;
  let increment = 0;
  for (i = 0; i < code_len; i++) {
    index *= 2;
    increment = (code[i] == '.')? 1 : 2;
    index += increment;
  }
  return lookup[ index ];
}


const encodeString = (input) => {
  const encodeChar = (input) => {
    const undefinedChar = '#';
    const undef = lookup.indexOf(undefinedChar) + 1;
    return [input]
      .map(c => c.toUpperCase())
      .map(c => (lookup.indexOf(c) + 1) || undef)
      .map(n => (n >>> 0).toString(2))
      .map(s => s.replace(/0/g, '.').replace(/1/g, '-'))
      .map(b => b.substring(1))
      [0];
  }

  return chars = input
    .split('')
    .map(c => encodeChar(c))
    .join(' ');
}

const encode = (input) => {
  const message = encodeString(input);
  const init  = '0000000';
  const space = '000';
  const intra = '0';
  const dot   = '1';
  const dash  = '111';
  const end   = '00000000 ';

  const signaled = message
    .replace(/ /g, space)
    .replace(/\./g, intra + dot)
    .replace(/-/g, intra + dash);

  const signal = `${init}${signaled}${end}`;

  return signal;
}

const compress = (signal) => {
  let count = 0;
  let lastC = null;
  const compressed = signal
    .split('')
    .map(c => {
      if (!lastC) {
        lastC = c;
        return null;
      } else if (c != lastC) {
        let cCount = count;
        let cOut = lastC;
        lastC = c;
        count = 1;
        return {
          c: cOut,
          count: cCount
        }
      } else {
        count = count + 1;
        return null;
      }
    })
    .filter(s => s);

  return compressed;
}

const mapToMilliseconds = (i, msDelay) => {
  return {
    ms: i.count * msDelay,
    action: (i.c === '0') ? 'OFF' : 'ON'
  }
}

const morseMessageTiming = (message, msDelay, callback) => {
  const encoded = encode(message);
  const states = compress(encoded).map(c => mapToMilliseconds(c, msDelay));

  const processStates = (states) => {
    if (states.length) {
      const {ms, action} = states.shift();
      callback(action);
      setTimeout(() => processStates(states), ms);
    }
  }

  processStates(states);
}

export default morseMessageTiming;

