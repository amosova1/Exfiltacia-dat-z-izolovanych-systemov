#define NOMINMAX // This must be before <windows.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

bool filter_applied = false;

void apply_filter(BYTE* pixel_data, BYTE* previous_pixel_data, int width, int height, int pitch, BYTE* overflow_data, int go) {
    // Modifikovateľné dáta
    int addred = 2, addgreen = 2, addblue = 2;
    int addred2 = -2,addgreen2 = -2, addblue2 = -2;

    int pixelHeight = height;
    int pixelWidth = width;

    int fromX = 0;
    int fromY = 0;
    //

    if (go == 1 && !filter_applied) {
        filter_applied = true;

        for (int y = fromY; y < pixelHeight; y++) {
            for (int x = fromX; x < pixelWidth; x++) {
                int offset = y * pitch + x * 4;

                BYTE b = pixel_data[offset];
                BYTE g = pixel_data[offset + 1];
                BYTE r = pixel_data[offset + 2];

                // Uloženie predchádzajúcej hodnoty pixelov
                previous_pixel_data[offset + 2] = r;
                previous_pixel_data[offset + 1] = g;
                previous_pixel_data[offset] = b;

                // Aplikovanie filtra
                int temp_r = r + addred;
                int temp_g = g + addgreen;
                int temp_b = b + addblue;

                // Vypočítanie pretečenia alebo podtečenia hodnoty pixelu po aplikovaní filtra
                overflow_data[offset + 2] = (temp_r > 255) ? (temp_r - 255) : ((temp_r < 0) ? -temp_r : 0);
                overflow_data[offset + 1] = (temp_g > 255) ? (temp_g - 255) : ((temp_g < 0) ? -temp_g : 0);
                overflow_data[offset] = (temp_b > 255) ? (temp_b - 255) : ((temp_b < 0) ? -temp_b : 0);

                // Zabezpečenie ohraničenia hodnoty RGB hodnoty pixelu na 0 až 255
                pixel_data[offset + 2] = (BYTE)std::max(0, std::min(255, temp_r));
                pixel_data[offset + 1] = (BYTE)std::max(0, std::min(255, temp_g));
                pixel_data[offset] = (BYTE)std::max(0, std::min(255, temp_b));
            }
        }
    }
    else if (go == 0 && filter_applied) { // vysielaná hodnota bitu je 0 pričom bol aplikovaný filter - je potrebné vrátiť hodnoty do pôvodného stavu
        filter_applied = false;
        
        for (int y = fromY; y < pixelHeight; y++) {
            for (int x = fromX; x < pixelWidth; x++) {
                int offset = y * pitch + x * 4;

                BYTE b = pixel_data[offset];
                BYTE g = pixel_data[offset + 1];
                BYTE r = pixel_data[offset + 2];

                //  Navrátenie pôvodnej hodnoty pixelov
                int temp_r = r + addred2;
                int temp_g = g + addgreen2;
                int temp_b = b + addblue2;

                // Vypočítanie pretečenia alebo podtečenia hodnoty pixelu po aplikovaní filtra
                temp_r = std::max(0, std::min(255, temp_r + overflow_data[offset + 2]));
                temp_g = std::max(0, std::min(255, temp_g + overflow_data[offset + 1]));
                temp_b = std::max(0, std::min(255, temp_b + overflow_data[offset]));

                // Uloženie nový hodnôt pixelov
                pixel_data[offset + 2] = (BYTE)temp_r;
                pixel_data[offset + 1] = (BYTE)temp_g;
                pixel_data[offset] = (BYTE)temp_b;

                // Uloženie dát ako predchádzajúce
                previous_pixel_data[offset + 2] = (BYTE)temp_r;
                previous_pixel_data[offset + 1] = (BYTE)temp_g;
                previous_pixel_data[offset] = (BYTE)temp_b;

                // Vynulovanie hodnôt pretečenia / podtečenia
                overflow_data[offset + 2] = 0;
                overflow_data[offset + 1] = 0;
                overflow_data[offset] = 0;
            }
        }
    }
    else {
        return;
    }
}


void update_screen(HDC hdc, HDC hMemDC, HBITMAP hBitmap, BYTE* pixel_data, BYTE* previous_pixel_data, BYTE* overflow_data, BITMAPINFO* bmpInfo, int width, int height, int iteration, int go) {
    // Zachytenie aktuálneho obrazu
    BitBlt(hMemDC, 0, 0, width, height, hdc, 0, 0, SRCCOPY);
    GetDIBits(hMemDC, hBitmap, 0, height, pixel_data, bmpInfo, DIB_RGB_COLORS);

    // Aplikovanie filtra
    apply_filter(pixel_data, previous_pixel_data, width, height, width * 4, overflow_data, go);

    // Nastavenie nových hodnôt pixelov na obraz
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

    // Demo data - heslo
    unsigned short value = 0b1101011011010110;

    for (int i = 15; i >= 0; --i) {
        DWORD startTime = GetTickCount();

        printf("Starting update %d with go=%d at %lu ms\n", i, ((value >> i) & 1), startTime);

        if ((value >> i) & 1) {
            update_screen(hdc, hMemDC, hBitmap, pixel_data, previous_pixel_data, overflow_data, &bmpInfo, width, height, i, 1);
        }
        else {
            update_screen(hdc, hMemDC, hBitmap, pixel_data, previous_pixel_data, overflow_data, &bmpInfo, width, height, i, 0);
        }

        DWORD endTime = GetTickCount();
        printf("Finished update %d at %lu ms, duration: %lu ms\n\n", i, endTime, endTime - startTime);

        Sleep(500);
    }

    free(pixel_data);
    free(previous_pixel_data);
    free(overflow_data);
    DeleteDC(hMemDC);
    DeleteObject(hBitmap);
    ReleaseDC(NULL, hdc);

    return 0;
}
