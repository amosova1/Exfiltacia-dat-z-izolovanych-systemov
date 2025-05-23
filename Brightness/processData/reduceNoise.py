import cv2
import numpy as np

# Modifikovateľné dáta
preparedVideoName = 'stabilized_videoF.mp4'
finalVideoName = 'noise_videoF.mp4'


# Načítanie pripraveného videa videa
cap = cv2.VideoCapture(preparedVideoName)

# Načítanie vlastností videa
frame_width = int(cap.get(3))
frame_height = int(cap.get(4))
fps = int(cap.get(cv2.CAP_PROP_FPS))

out = cv2.VideoWriter(finalVideoName, cv2.VideoWriter_fourcc(*'mp4v'), fps, (frame_width, frame_height))

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        # Ukončenie cyklu po skončení videa
        break 

    # Aplikovanie bilaterálneho filtra (odstránenie vizuálneho šumu so zachovaním ostrosti)
    bilateral = cv2.bilateralFilter(frame, d=9, sigmaColor=75, sigmaSpace=75)

    # Aplikovanie nelokálneho priemerného šumu (vyššia hodnota h určuje intenzitu redukcie šumu)
    denoised = cv2.fastNlMeansDenoisingColored(bilateral, None, 3, 3, 7, 21)

    # Aplikovanie mediánového rozmazania (odstráni nekonzistentné pixely / "bodky")
    final_output = cv2.medianBlur(denoised, 1)

    # Zápis upraveného snímku
    out.write(final_output)

cap.release()
out.release()
cv2.destroyAllWindows()
