import cv2
import numpy as np

cap = cv2.VideoCapture("video.mp4")

# Get video properties
n_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
fps = cap.get(cv2.CAP_PROP_FPS)

# Define the output video writer
out = cv2.VideoWriter("stabilized_video.mp4", cv2.VideoWriter_fourcc(*'mp4v'), fps, (frame_width, frame_height))

# Read the first frame
ret, prev_frame = cap.read()
prev_gray = cv2.cvtColor(prev_frame, cv2.COLOR_BGR2GRAY)

# Initialize transformation matrix list
transforms = np.zeros((n_frames-1, 3), np.float32)

for i in range(n_frames-1):
    ret, curr_frame = cap.read()
    if not ret:
        break

    curr_gray = cv2.cvtColor(curr_frame, cv2.COLOR_BGR2GRAY)

    # Detect feature points
    prev_pts = cv2.goodFeaturesToTrack(prev_gray, maxCorners=200, qualityLevel=0.01, minDistance=30, blockSize=3)
    
    # Track feature points
    curr_pts, status, _ = cv2.calcOpticalFlowPyrLK(prev_gray, curr_gray, prev_pts, None)

    # Filter valid points
    idx = np.where(status == 1)[0]
    prev_pts = prev_pts[idx]
    curr_pts = curr_pts[idx]

    # Estimate transformation
    m = cv2.estimateAffinePartial2D(prev_pts, curr_pts)[0]

    dx = m[0, 2]
    dy = m[1, 2]
    da = np.arctan2(m[1, 0], m[0, 0])
    
    transforms[i] = [dx, dy, da]

    prev_gray = curr_gray

# Apply moving average filter to smooth the transformations
smoothed_trajectory = np.cumsum(transforms, axis=0)
smoothed_trajectory -= smoothed_trajectory.mean(axis=0)

# Apply stabilization
cap.set(cv2.CAP_PROP_POS_FRAMES, 0)
ret, first_frame = cap.read()
out.write(first_frame)

for i in range(n_frames-1):
    ret, frame = cap.read()
    if not ret:
        break

    dx, dy, da = transforms[i] - smoothed_trajectory[i]

    # Create transformation matrix
    m = np.array([[np.cos(da), -np.sin(da), dx],
                  [np.sin(da),  np.cos(da), dy]])

    # Apply transformation
    stabilized_frame = cv2.warpAffine(frame, m, (frame_width, frame_height))

    # Write frame to output
    out.write(stabilized_frame)

cap.release()
out.release()
cv2.destroyAllWindows()
