import cv2

# Open the video
cap = cv2.VideoCapture("output_video.mp4")

# Check if the video opened successfully
if not cap.isOpened():
    print("Error: Could not open video.")
    exit()

# File to save pixel values with timestamps
output_file = open("pixel_values.txt", "w")
output_file.write("Time(ms), PixelValue\n")

while True:
    ret, frame = cap.read()
    
    if not ret:
        break  # Exit loop when video ends

    # Get the timestamp of the current frame (in milliseconds)
    time_ms = cap.get(cv2.CAP_PROP_POS_MSEC)

    # Get the pixel value at (y=400, x=15)
    pixel_value = frame[400, 15]  # OpenCV uses (row, col) -> (y, x)
    
    # Save time and pixel value to file
    output_file.write(f"{time_ms:.2f}, {pixel_value}\n")

    # Optional - display in real time  
        # Draw a red dot at (15, 400)
    #cv2.circle(frame, (15, 400), radius=5, color=(0, 0, 255), thickness=-1)

        # Display the frame
    #cv2.imshow("Video with Red Dot", frame)

    # Exit if 'q' is pressed
        # if cv2.waitKey(25) & 0xFF == ord('q'):
            # break

output_file.close()
cap.release()
cv2.destroyAllWindows()

print("Pixel extraction completed. Values with timestamps saved to pixel_values.txt.")
