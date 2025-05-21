#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void apply_filter(BYTE* pixel_data, BYTE* previous_pixel_data, int width, int height, int pitch, BYTE* overflow_data, int go) {
    // Set changes in RGP value for pixels when value of data is 1
    int addred = 4;
    int addblue = 4;
    int addgreen = 4;

    // Set changes in RGP value for pixels when value of data is 0
    int addred2 = -4;
    int addblue2 = -4;
    int addgreen2 = -4;

    // Set height and width of pixels which will generate signals
    int pixelHeight = 200;
    int pixelWidth = 10;

    for (int y = 100; y < pixelHeight; y++) {
        for (int x = 0; x < pixelWidth; x++) {
            int pixel_offset = (y * pitch) + (x * 4); // BGRA format
            BYTE b = pixel_data[pixel_offset];
            BYTE g = pixel_data[pixel_offset + 1];
            BYTE r = pixel_data[pixel_offset + 2];

            if (go == 1) {
                // Check if (r - 50) matches previous red value
                if ((overflow_data[pixel_offset + 2] == 0 && previous_pixel_data[pixel_offset + 2] != (BYTE)max(0, r - addred))
                    || (overflow_data[pixel_offset + 2] != 0 && previous_pixel_data[pixel_offset + 2] != (BYTE)max(0, r - (addred - overflow_data[pixel_offset + 2]))) ||
                    (overflow_data[pixel_offset + 1] == 0 && previous_pixel_data[pixel_offset + 1] != (BYTE)max(0, g - addgreen))
                    || (overflow_data[pixel_offset + 1] != 0 && previous_pixel_data[pixel_offset + 1] != (BYTE)max(0, g - (addgreen - overflow_data[pixel_offset + 1]))) ||
                    (overflow_data[pixel_offset] == 0 && previous_pixel_data[pixel_offset] != (BYTE)max(0, b - addblue))
                    || (overflow_data[pixel_offset] != 0 && previous_pixel_data[pixel_offset] != (BYTE)max(0, b - (addblue - overflow_data[pixel_offset])))) {

                    // Save original values before applying filter
                    previous_pixel_data[pixel_offset + 2] = r;
                    previous_pixel_data[pixel_offset + 1] = g;
                    previous_pixel_data[pixel_offset] = b;

                    // Set filter
                    BYTE new_r = (BYTE)min(255, r + addred);
                    BYTE new_g = (BYTE)min(255, g + addgreen);
                    BYTE new_b = (BYTE)min(255, b + addblue);

                    // Check if it crosses 255
                    if (new_r < r + addred) {
                        overflow_data[pixel_offset + 2] = (r + addred) - 255;  // Store the overflow amount
                    }
                    else {
                        overflow_data[pixel_offset + 2] = 0;  // No overflow
                    }

                    // Check if it crosses 255
                    if (new_g < g + addgreen) {
                        overflow_data[pixel_offset + 1] = (g + addgreen) - 255;  // Store the overflow amount
                    }
                    else {
                        overflow_data[pixel_offset + 1] = 0;  // No overflow
                    }

                    // Check if it crosses 255
                    if (new_b < b + addblue) {
                        overflow_data[pixel_offset] = (b + addblue) - 255;  // Store the overflow amount
                    }
                    else {
                        overflow_data[pixel_offset] = 0;  // No overflow
                    }

                    // Apply filter
                    r = new_r;
                    b = new_b;
                    g = new_g;
                }

                // Update pixel data
                pixel_data[pixel_offset] = b;
                pixel_data[pixel_offset + 1] = g;
                pixel_data[pixel_offset + 2] = r;
            }
            else {
                // Save original values before applying filter
                previous_pixel_data[pixel_offset + 2] = r;
                previous_pixel_data[pixel_offset + 1] = g;
                previous_pixel_data[pixel_offset] = b;

                // Set filter
                BYTE new_r = (BYTE)min(255, r + addred2);
                BYTE new_g = (BYTE)min(255, g + addgreen2);
                BYTE new_b = (BYTE)min(255, b + addblue2);

                // Check if it crosses 255
                if (new_r < r + addred2) {
                    overflow_data[pixel_offset + 2] = (r + addred2) - 255;  // Store the overflow amount
                }
                else {
                    overflow_data[pixel_offset + 2] = 0;  // No overflow
                }

                // Check if it crosses 255
                if (new_g < g + addgreen2) {
                    overflow_data[pixel_offset + 1] = (g + addgreen2) - 255;  // Store the overflow amount
                }
                else {
                    overflow_data[pixel_offset + 1] = 0;  // No overflow
                }

                // Check if it crosses 255
                if (new_b < b + addblue2) {
                    overflow_data[pixel_offset] = (b + addblue2) - 255;  // Store the overflow amount
                }
                else {
                    overflow_data[pixel_offset] = 0;  // No overflow
                }

                // Apply filter
                r = new_r;
                b = new_b;
                g = new_g;

                // Update pixel data
                pixel_data[pixel_offset] = b;
                pixel_data[pixel_offset + 1] = g;
                pixel_data[pixel_offset + 2] = r;
            }
        }
    }
}


void update_screen(HDC hdc, HDC hMemDC, HBITMAP hBitmap, BYTE* pixel_data, BYTE* previous_pixel_data, BYTE* overflow_data, BITMAPINFO* bmpInfo, int width, int height, int iteration, int go) {
    // Capture the current screen
    BitBlt(hMemDC, 0, 0, width, height, hdc, 0, 0, SRCCOPY);
    GetDIBits(hMemDC, hBitmap, 0, height, pixel_data, bmpInfo, DIB_RGB_COLORS);

    // Apply red filter conditionally
    apply_filter(pixel_data, previous_pixel_data, width, height, width * 4, overflow_data, go);

    // Apply modified bitmap to the screen
    SetDIBits(hMemDC, hBitmap, 0, height, pixel_data, bmpInfo, DIB_RGB_COLORS);
    BitBlt(hdc, 0, 0, width, height, hMemDC, 0, 0, SRCCOPY);
}

int main() {
    HDC hdc = GetDC(NULL);
    if (!hdc) {
        printf("Error getting device context\n");
        return -1;
    }

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
    if (!hBitmap) {
        printf("Error creating bitmap\n");
        ReleaseDC(NULL, hdc);
        return -1;
    }

    HDC hMemDC = CreateCompatibleDC(hdc);
    SelectObject(hMemDC, hBitmap);

    BITMAPINFO bmpInfo = { 0 };
    bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
    bmpInfo.bmiHeader.biWidth = width;
    bmpInfo.bmiHeader.biHeight = -height;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB;


    int pitch = width * 4;
    BYTE* pixel_data = (BYTE*)malloc(pitch * height);
    BYTE* previous_pixel_data = (BYTE*)malloc(pitch * height);
    BYTE* overflow_data = (BYTE*)malloc(pitch * height);
    if (!pixel_data || !previous_pixel_data || !overflow_data) {
        printf("Error allocating memory\n");
        if (pixel_data) free(pixel_data);
        if (previous_pixel_data) free(previous_pixel_data);
        if (overflow_data) free(overflow_data);
        ReleaseDC(NULL, hdc);
        return -1;
    }

    memset(previous_pixel_data, 0, pitch * height);
    memset(overflow_data, 0, pitch * height);

    // Demo data - password
    unsigned char value = 0b11010110;

    for (int i = 7; i >= 0; --i) {
        if ((value >> i) & 1) {
            update_screen(hdc, hMemDC, hBitmap, pixel_data, previous_pixel_data, overflow_data, &bmpInfo, width, height, i, 1);
            Sleep(1000);
        }
        else {
            update_screen(hdc, hMemDC, hBitmap, pixel_data, previous_pixel_data, overflow_data, &bmpInfo, width, height, i, 0);
            Sleep(1000);
        }
    }

    free(pixel_data);
    free(previous_pixel_data);
    free(overflow_data);
    DeleteDC(hMemDC);
    DeleteObject(hBitmap);
    ReleaseDC(NULL, hdc);

    return 0;
}
