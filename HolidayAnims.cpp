#include "HolidayAnims.h"
#include "MatrixDisplay.h"

HolidayAnims::HolidayAnims(MatrixDisplay* disp)
: display(disp) {}

void HolidayAnims::beginAll() {
    // Register built-in animations here
}

void HolidayAnims::updateAll() {
    // Animation tick loop (intentionally light)
}
