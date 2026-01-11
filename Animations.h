/* Animations.h
   Procedural animations header
   VERSION: V16.2.3-2026-01-10T21:40:00Z - Namespace-based (NO CLASS)
*/

#pragma once

#include <Arduino.h>
#include "MatrixDisplay.h"

// Base class for animation framework (not used by procedurals)
class Animation {
public:
    explicit Animation(MatrixDisplay* display) : matrix(display) {}
    virtual ~Animation() = default;
    virtual void begin() {}
    virtual void update() = 0;

protected:
    MatrixDisplay* matrix;
};

// V16.2.3-2026-01-10T21:40:00Z - Procedural animation namespace (NOT a class!)
namespace Animations {
    void chase(MatrixDisplay* disp);
    void snowfall(MatrixDisplay* disp);
    void snowfallGentle(MatrixDisplay* disp);
    void snowfallHeavy(MatrixDisplay* disp);
    void sparklingStars(MatrixDisplay* disp);
}
