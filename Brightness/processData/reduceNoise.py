import cv2
import numpy as np

cap = cv2.VideoCapture('video2.mp4')

# Get video properties
frame_width = int(cap.get(3))
frame_height = int(cap.get(4))
fps = int(cap.get(cv2.CAP_PROP_FPS))

# Define codec and create VideoWriter object
out = cv2.VideoWriter('output_video.mp4', cv2.VideoWriter_fourcc(*'mp4v'), fps, (frame_width, frame_height))

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break  # Break loop if no frame is captured

    # Apply Bilateral Filter (removes noise while keeping edges sharp)
    bilateral = cv2.bilateralFilter(frame, d=9, sigmaColor=75, sigmaSpace=75)

    # Apply Non-Local Means Denoising (higher h values to remove pixel noise)
    denoised = cv2.fastNlMeansDenoisingColored(bilateral, None, 3, 3, 7, 21)

    # Apply Median Blur (removes tiny dots/pixels)
    final_output = cv2.medianBlur(denoised, 1)

    # Write the frame to the output video
    out.write(final_output)

    # Optional - Display the frame
        # cv2.imshow('Denoised Video', final_output)
        # if cv2.waitKey(1) & 0xFF == ord('q'):
            # break

# Release resources
cap.release()
out.release()
cv2.destroyAllWindows()
