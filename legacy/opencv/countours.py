import cv2

def display_img( img, delay=1000 ):
    cv2.imshow("preview", img)
    cv2.waitKey( delay )
    cv2.destroyWindow("preview")

im = cv2.imread('phone_on.png')
display_img( im )
imgray = cv2.cvtColor(im,cv2.COLOR_BGR2GRAY)

display_img( imgray )

ret,thresh = cv2.threshold(imgray,127,255,0)

display_img( thresh )

image, contours = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)

cnt = contours[0]

print cnt
#x,y,w,h = cv2.boundingRect(cnt)
#result = cv2.boundingRect(cnt)
#img = cv2.rectangle(image,(x,y),(x+w,y+h),(0,255,0),2)

