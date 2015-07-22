"""
short short short long: sync
long: 
    invert
    output
short:
    short: 
        output
"""

class ManchesterDecoder:
    def __init__( self ):
        self.msg = []
        self.bit = 0
        self.was_short = False

    def short( self ):
        if self.was_short:
            self.was_short = False
            self.msg.append( str( self.bit ) )
        else:
            self.was_short = True

    def long( self ):
        if self.was_short:
            print "SYNC ERROR"
        else:
            self.msg.append( str( self.bit ) )

            self.was_short = False

            if self.bit == 0:
                self.bit = 1
            else:
                self.bit = 0

    def get_message( self ):
        return "".join( self.msg )

md = ManchesterDecoder()

for k in "LLSSLSS":
    if k == "L":
        md.long()
    else:
        md.short()

print md.get_message()
