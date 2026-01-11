#pragma once

#include <Arduino.h>

class MatrixDisplay;

class HolidayAnims {
public:
    HolidayAnims(MatrixDisplay* disp);

    void beginAll();
    void updateAll();

private:
    MatrixDisplay* display;
};
