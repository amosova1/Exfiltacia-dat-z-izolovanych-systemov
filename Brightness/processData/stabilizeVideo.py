import cv2
import numpy as np

# Open the video file
cap = cv2.VideoCapture("videoFFF.mp4")

# Get video properties
n_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
fps = cap.get(cv2.CAP_PROP_FPS)

# Prepare video writer
out = cv2.VideoWriter("stabilized_videoF.mp4", cv2.VideoWriter_fourcc(*'mp4v'), fps, (frame_width, frame_height))

# Read the first frame
ret, prev_frame = cap.read()
prev_gray = cv2.cvtColor(prev_frame, cv2.COLOR_BGR2GRAY)

# Store transforms: dx, dy, da
transforms = []

for _ in range(n_frames - 1):
    ret, curr_frame = cap.read()
    if not ret:
        break

    curr_gray = cv2.cvtColor(curr_frame, cv2.COLOR_BGR2GRAY)

    prev_pts = cv2.goodFeaturesToTrack(prev_gray, maxCorners=200, qualityLevel=0.01, minDistance=30, blockSize=3)
    curr_pts, status, _ = cv2.calcOpticalFlowPyrLK(prev_gray, curr_gray, prev_pts, None)

    if prev_pts is None or curr_pts is None or status is None:
        transforms.append([0, 0, 0])
        prev_gray = curr_gray
        continue

    valid = status.flatten() == 1
    prev_pts = prev_pts[valid].reshape(-1, 2)
    curr_pts = curr_pts[valid].reshape(-1, 2)

    if len(prev_pts) < 10:
        transforms.append([0, 0, 0])
        prev_gray = curr_gray
        continue

    m, _ = cv2.estimateAffinePartial2D(prev_pts, curr_pts)
    if m is None:
        transforms.append([0, 0, 0])
        prev_gray = curr_gray
        continue

    dx = m[0, 2]
    dy = m[1, 2]
    da = np.arctan2(m[1, 0], m[0, 0])
    transforms.append([dx, dy, da])

    prev_gray = curr_gray

# Convert to numpy array
transforms = np.array(transforms)

# Compute trajectory
trajectory = np.cumsum(transforms, axis=0)

# Smooth trajectory
def moving_average(curve, radius=50):
    window_size = 2 * radius + 1
    kernel = np.ones(window_size) / window_size
    curve_pad = np.pad(curve, ((radius, radius), (0, 0)), mode='edge')
    smooth = np.array([np.convolve(curve_pad[:, i], kernel, mode='valid') for i in range(curve.shape[1])]).T
    return smooth

smoothed_trajectory = moving_average(trajectory)

# Compute difference
difference = smoothed_trajectory - trajectory
transforms_smooth = transforms + difference

# Reapply transforms to stabilize
cap.set(cv2.CAP_PROP_POS_FRAMES, 0)
ret, frame = cap.read()
out.write(frame)

for i in range(n_frames - 1):
    ret, frame = cap.read()
    if not ret:
        break

    dx, dy, _ = transforms_smooth[i]  # remove rotation
    da = 0  # force no rotation

    m = np.array([
        [np.cos(da), -np.sin(da), dx],
        [np.sin(da),  np.cos(da), dy]
    ])

    stabilized = cv2.warpAffine(frame, m, (frame_width, frame_height))

    # Crop & zoom to avoid border black bars
    zoom = 1.05
    h, w = stabilized.shape[:2]
    center_x, center_y = w // 2, h // 2
    new_w, new_h = int(w / zoom), int(h / zoom)

    x1 = center_x - new_w // 2
    y1 = center_y - new_h // 2
    x2 = x1 + new_w
    y2 = y1 + new_h

    stabilized = stabilized[y1:y2, x1:x2]
    stabilized = cv2.resize(stabilized, (frame_width, frame_height))

    out.write(stabilized)

cap.release()
out.release()
cv2.destroyAllWindows()
