function addToCRC8 ( crc, data ) {
    var extract = data;
    for (var i = 0; i < 8; i++) {
        var sum = ( crc ^ extract ) & 0x01;
        crc >>= 1;
        if (sum) {
            crc ^=0x8C;
        };
        extract >>= 1;
    };
    return crc;
}

function calcCRC8 ( msg ) {
    var crc = 0;
    for (var i = 0; i < msg.length; i++) {
        var charCode = msg.charCodeAt( i );
        crc = addToCRC8( crc, charCode );
    };
    return crc;
}

function CRC8Char( msg ) {
    var crc8 = calcCRC8( msg );
    return String.fromCharCode( crc8 );
}

function zeroBoundary () {
    return "000000000000000";
}

function toBin( number ) {
    var n = "00000000" + number.toString(2);
    return n.substr(n.length - 8);
}

function expandBit ( bit ) {
    var signal_base_len = 3;
    var noise = Math.random() > 0.5 ? 1: 0;
    var signal_len = signal_base_len + noise;
    var expanded = "";

    for (var i = 0; i < signal_len; i++) {
        expanded += bit;
    };

    return expanded;
}

function encodeBit( bit ) {
    var encoded = "";
    switch( bit ) {
        case '0':
            encoded += expandBit( '1' );
            encoded += expandBit( '0' );
            break;
        case '1':
            encoded += expandBit( '0' );
            encoded += expandBit( '1' );
            break;
        default: // nothing to do
    }
    return encoded;
}

function manchesterEncode( message ) {
    var preamble = String.fromCharCode( 0xFE );
    var crc8 = CRC8Char( message );

    var msg = preamble + message + crc8;
    
    var encoded = "";
    for( var char_index = 0; char_index < msg.length; char_index++ ) {
        charCode = msg.charCodeAt( char_index );
        bin = toBin( charCode );

        for (var bit_index = 0; bit_index < bin.length; bit_index++) {
            encoded += encodeBit( bin[ bit_index ] );
        };
    }
    encoded = zeroBoundary() + encoded + zeroBoundary();
    return encoded;
}

mEncoded = manchesterEncode( "hello" );


function manchesterDecode( stream ) {
    sync = false;
    preamble_start = false;
    bit_count = 0;
    bit_last = 0;

    up_len = 3;
    long_time = up_len * 2;
    timeout_time = up_len * 4;

    bit_waste = false;
    sync_bit = 0;
    half_bit = false;

    out = "";

    for (var i = 0; i < stream.length; i++) {
        bit = stream[i];

        if (!sync) {
            if ( bit != bit_last ) {
                if (bit_count >= long_time) {
                    if (preamble_start) {
                        sync = true;
                    };
                };
                bit_count = 0;
            };
            if (bit == '1' ) {
                preamble_start = true;
            };
        }
        else {
            if ( bit_count > timeout_time ) {
                sync = false;
                bit_count = 0;
            };
            if ( bit != bit_last ) {
                if ( !bit_waste ) {
                    bit_waste = true;
                    sync_bit = bit;
                } else {
                    if ( bit_count >= long_time ) {
                        half_bit = false;
                        if ( sync_bit == '0' ) {
                            sync_bit = '1';
                        } else {
                            sync_bit = '0';
                        };
                        out += sync_bit;
                    } else {
                        if ( !half_bit ) {
                            half_bit = true;
                        } else {
                            half_bit = false;
                            out += sync_bit;
                        };
                    };
                    bit_count = 0;
                };
            };
        };

        bit_count += 1;
        bit_last = bit;
    };

    byte_buffer = "";
    byte_count = 0;
    msg_out = "";

    for (var i = 0; i < out.length; i++) {
        byte_buffer += out[i];

        if ( byte_count >= 7 ) {
            byte_count = 0;
            msg_out += String.fromCharCode(parseInt(byte_buffer,2) );
            byte_buffer = "";
        } else {
            byte_count += 1;
        };
    };

    msg = msg_out.substring( 0, msg_out.length -1);
    msg_crc = msg_out.charCodeAt( msg_out.length -1 );

    if( msg_crc == calcCRC8 ( msg ) ) {
        console.log( msg );
    } else {
        console.log( "checksum error" );
    };

}

manchesterDecode( mEncoded );
