import cv2

# Modifikovateľné dáta
x_pos = 800
y_pos = 50

preparedVideoName = "noise_videoF.mp4"
finalData = "pixel_values.txt"

cap = cv2.VideoCapture(preparedVideoName)

if not cap.isOpened():
    print("Error: Could not open video.")
    exit()

# Súbor na zapísanie získaných dát
output_file = open(finalData, "w")
output_file.write("Time(ms), PixelValue\n")

while True:
    ret, frame = cap.read()
    
    if not ret:
        # Ukončenie cyklu - video skončilo
        break 

    # Záskanie času snímku v milisekundách
    time_ms = cap.get(cv2.CAP_PROP_POS_MSEC)

    # Získanie hodnôt pixelu na pozícii (y_pos, x_pos) - OpenCV používa (riadok, stĺpec) -> (y, x)
    pixel_value = frame[y_pos, x_pos] 
    
    output_file.write(f"{time_ms:.2f}, {pixel_value}\n")

    print(f"{time_ms:.2f}, {pixel_value}\n")

    # Vizuálne zaznačenie vybraného pixelu (x_pos, y_pos)
    cv2.circle(frame, (x_pos, y_pos), radius=5, color=(0, 0, 255), thickness=-1)
    cv2.imshow("Video with Red Dot", frame)

    if cv2.waitKey(25) & 0xFF == ord('q'):
        break

output_file.close()

cap.release()
cv2.destroyAllWindows()

print("Pixel extraction completed. Values with timestamps saved to pixel_values.txt.")
