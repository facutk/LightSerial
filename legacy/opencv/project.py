import cv2
import time

class MorseAnalyser():
    def __init__(self):
        self.state_prev = ""
        self.state_count = 0
        self.dash_len = 0
        self.stop_len = 0
        self.code_buffer = ""
        self.last_time = time.time()
        self.dic = {
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

    def tochar( self, code ):
        if self.dic.has_key(code):
            return self.dic[code]
        return "?"


    def feed_state(self, state):
        #print state
        if state == self.state_prev:
            if self.dash_len < 6:
                if state == '=' and self.state_count > self.dash_len:
                    self.dash_len = self.state_count
                    self.stop_len = 2*self.dash_len
            self.state_count += 1
        else:
            if self.state_count > self.stop_len:
                # es un stop
                #print self.code_buffer
                codes = self.code_buffer.split(' ')
                self.char_buffer = ""
                for code in codes:
                    self.char_buffer += self.tochar( code )
                print self.char_buffer
                time_now = time.time()
                print time_now - self.last_time
                self.last_time = time_now 
                self.code_buffer = ""
            else:
                is_long = self.state_count >= self.dash_len
                if self.state_prev == '=':
                    if is_long:
                        self.code_buffer += '-'
                    else:
                        self.code_buffer += '.'
                else:
                    if is_long:
                        self.code_buffer += ' '
                print self.code_buffer
            self.state_count = 1
        self.state_prev = state

def grey_histogram(img, nBins=64):
    """
    Returns a one dimension histogram for the given image
    The image is expected to have one channel, 8 bits depth
    nBins can be defined between 1 and 255 
    """
    hist_size = [nBins]
    #h_ranges = [0, 255]
    hist = cv2.createHist(hist_size , cv2.CV_HIST_ARRAY, [[0, 255]], 1)
    cv2.calcHist([img], hist)

    return hist

def extract_bright(grey_img, histogram=False):
    """
    Extracts brightest part of the image.
    Expected to be the LEDs (provided that there is a dark background)
    Returns a Thresholded image
    histgram defines if we use the hist calculation to find the best margin
    """
    ## Searches for image maximum (brightest pixel)
    # We expect the LEDs to be brighter than the rest of the image
    [minVal, maxVal, minLoc, maxLoc] = cv2.minMaxLoc(grey_img)
    #print "Brightest pixel val is %d" %(maxVal)
    state = ""
    if maxVal == 255:
        state = '='
    else:
        state = '.'
    
    return state
    #We retrieve only the brightest part of the image
    # Here is use a fixed margin (80%), but you can use hist to enhance this one    
    if 0:
        ## Histogram may be used to wisely define the margin
        # We expect a huge spike corresponding to the mean of the background
        # and another smaller spike of bright values (the LEDs)
        hist = grey_histogram(grey_img, nBins=64)
        [hminValue, hmaxValue, hminIdx, hmaxIdx] = cv2.getMinMaxHistValue(hist) 
        margin = 0# statistics to be calculated using hist data    
    else:  
        margin = 0.8
        
    thresh = int( maxVal * margin) # in pix value to be extracted
    #print "Threshold is defined as %d" %(thresh)
    return thresh
    #thresh = 204

    #thresh_img = cv2.CreateImage(cv2.GetSize(grey_img), grey_img.depth, 1)
    #cv2.Threshold(grey_img, thresh_img , thresh, 255, cv2.CV_THRESH_BINARY)

    #ret, thresh_img = cv2.threshold( grey_img,50,255,cv2.THRESH_BINARY)
    #return thresh_img
    #return thresh_img

def find_leds(thresh_img):
    """
    Given a binary image showing the brightest pixels in an image, 
    returns a result image, displaying found leds in a rectangle
    """
    """
    contours,hierarchy = cv2.findContours(thresh_img, 
                               mode=cv2.cv.CV_RETR_EXTERNAL , 
                               method=cv2.cv.CV_CHAIN_APPROX_NONE , 
                               offset=(0, 0))
    """



    # Find the contours
    contours,hierarchy = cv2.findContours(thresh_img, cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)

    hierarchy = hierarchy[0] # get the actual inner list of hierarchy descriptions

    # For each contour, find the bounding rectangle and draw it
    for component in zip(contours, hierarchy):
        currentContour = component[0]
        currentHierarchy = component[1]
        x,y,w,h = cv2.boundingRect(currentContour)
        if currentHierarchy[2] < 0:
            # these are the innermost child components
            cv2.rectangle( thresh_img ,(x,y),(x+w,y+h),(0,0,255),3)
        elif currentHierarchy[3] < 0:
            # these are the
            # outermost parent
            # components
            cv2.rectangle( thresh_img,(x,y),(x+w,y+h),(0,255,0),3)


    """
    regions = []
    for cnt in contours:
        pts = [ pt for pt in cnt ]
        x, y = zip(*pts)    
    exit()
    out_img = cv2.createImage(cv2.getSize( thresh_img ), 8, 3)
    """
    """
    while contours:
        pts = [ pt for pt in contours ]
        print pts
        x, y = zip(*pts)    
        min_x, min_y = min(x), min(y)
        width, height = max(x) - min_x + 1, max(y) - min_y + 1
        regions.append((min_x, min_y, width, height))
        contours = contours.h_next()

        out_img = cv2.createImage(cv2.getSize( thresh_img ), 8, 3)
    """
    """
    for x,y,width,height in regions:
        pt1 = x,y
        pt2 = x+width,y+height
        color = (0,0,255,0)
        cv2.rectangle(out_img, pt1, pt2, color, 2)
    """

    #return out_img, regions
    return thresh_img

cv2.namedWindow("preview")
vc = cv2.VideoCapture(0)
analyser = MorseAnalyser()

if vc.isOpened(): # try to get the first frame
    rval, frame = vc.read()
else:
    rval = False

while rval:
    # Imagen a Gris
    #frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    gray = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
    state = extract_bright(gray)
    analyser.feed_state( state )
    """
    thresh = extract_bright(gray)
    blur = cv2.GaussianBlur(gray,(5,5),0)
    #thresh = 204
    ret, frame = cv2.threshold(frame,thresh,255,cv2.THRESH_BINARY)

    edges = cv2.Canny(blur,thresh,thresh*2)
    drawing = np.zeros(frame.shape,np.uint8) # Image to draw the contours
    contours,hierarchy = cv2.findContours(edges,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
    for cnt in contours:
        color = np.random.randint(0,255,(3)).tolist() # Select a random color
        cv2.drawContours(drawing,[cnt],0,color,2)
        #cv2.imshow('output',drawing)
    #cv2.imshow('input',img)

    #led_img, regions = find_leds(frame)
    #contours,hierarchy = cv2.findContours( frame, cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
    #cnt = contours[0]

    #x,y,w,h = cv2.boundingRect(cnt)
    #print x, y, w, h
    #frame = cv2.rectangle( frame,(x,y),(x+w,y+h),(255,0,0),5)

    cv2.imshow("preview", drawing)
"""
    cv2.imshow("preview", gray)
    rval, frame = vc.read()
    key = cv2.waitKey(20)
    if key == 27: # exit on ESC
        break
cv2.destroyWindow("preview")
