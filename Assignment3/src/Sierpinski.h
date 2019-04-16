#ifndef Sierpinski_Included
#define Sierpinski_Included

#include "gwindow.h"
#include "gobjects.h"

/**
 * Draws a Sierpinski triangle with the specified corners.
 *
 * An order-0 Sierpinski triangle is just a regular filled triangle. An order-n Sierpinski
 * triangle, for n > 0, consists of three smaller Sierpinski triangles of order n-1, each half
 * as wide and as tall as the original triangle, positioned so that they meet corner-to-corner.
 *
 * The order of the provided points is irrelevant.
 *
 * If the order provided to this function is negative, this function should call error() to report
 * an error.
 *
 * @param window The window in which to draw the triangle.
 * @param p0     The first of the three triangle points
 * @param p1     The second of the three triangle points.
 * @param p2     The third of the three triangle points.
 * @param order  The order of the triangle, which will always be nonnegative.
 */
void drawSierpinskiTriangle(GWindow& window,
                            GPoint p0, GPoint p1, GPoint p2,
                            int order);

#endif
