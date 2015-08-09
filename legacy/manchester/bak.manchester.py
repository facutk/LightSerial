"""
1. Set up timer to interrupt every 2T / S
2. SR routine should check and store the state of the microcontroller
   pin (1 or 0)
3. Repeat step 2 for desired number of bits * S occurrences
4. Process through the captured buffer counting the number of
   consecutive ones or zeros
    5. When the next logic value changes
    a. Check if count >= (S/2); Then skip to step 6
    b. Else reset count and loop to step 4
6. Set current bit = logic value in buffer currently pointed too
7. Reset count and count to the next logic change
    a. Compare count with (S/2)
    b. If count < (S/2)
        i. Reset and count to next logic change
        ii. Make sure count also < (S/2)
        iii. Next bit = current bit
        iv. Store next bit in data buffer
    c. Else if count >= (S/2)
        i. Next bit = opposite of current bit
        ii. Store next bit in data buffer
    d. Else
        i. Return error
8. Loop to step 7 until completely through captured data
9. Exit for further data processing
"""



"""

import unittest

class TestManchester(unittest.TestCase):

    def setUp(self):
        self.mn = Manchester()

    def test_new_Manchester_has_no_data(self):
        self.assertFalse( self.mn.dataAvailable() )

    def test_new_Manchester_no_sync(self):
        self.assertFalse( self.mn.onSync() )

    def test_all_zero_still_no_sync(self):
        for k in range(150):
            self.mn.feed( 0 )
        self.assertFalse( self.mn.onSync() )


    def test_feed_00110011_still_no_sync(self):
        for k in "00110011":
            self.mn.feed( int( k ) )
        self.assertFalse( self.mn.onSync() )
        
    def test_feed_00110011110__synced(self):
        for k in "00110011110":
            self.mn.feed( k )
        self.assertTrue( self.mn.onSync() )
        """

class Manchester():

    #samples = [ 0 for x in range( sampling ) ]



    def __init__( self ):
        self.sampling = 2
        self.sampling_low = self.sampling - self.sampling/2
        self.sampling_high = self.sampling + self.sampling/2

        self.last_signal = None
        self.count = 0
        self.synced = False
        self.current_bit = None
        self.half_bit = False

    def getCount(self):
        return self.count

    def dataAvailable(self):
        return False

    def feed( self, signal ):
        if self.synced == False:
            if signal == self.last_signal:
                self.count += 1
            else:
                if self.count >= self.sampling_high:
                    self.synced = True
                    self.current_bit = signal
                self.count = 1
        else:
            if signal == self.last_signal:
                self.count += 1
            else:
                if self.half_bit:
                    if self.count < self.sampling_high:
                        self.current_bit = signal
                        print self.current_bit
                if self.count < self.sampling_high:
                    if self.half_bit == False:
                        self.half_bit = True
                        self.count = 1
                    self.current_bit = signal

                else:
                    print self.current_bit

                    if self.current_bit == "0":
                        self.current_bit = "1"
                    else:
                        self.current_bit = "0"

                    self.count = 1


        self.last_signal = signal

    def onSync( self ):
        return self.synced





if __name__ == '__main__':
    #unittest.main()
    mn = Manchester()

    #SYNC + 0100
    for k in "001111000011110011001":
        mn.feed( k )
