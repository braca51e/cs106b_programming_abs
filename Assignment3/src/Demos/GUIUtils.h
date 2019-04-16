#ifndef GUIUtils_Included
#define GUIUtils_Included

#include "TemporaryComponent.h"
#include "gobjects.h"
#include "ginteractors.h"
#include "gwindow.h"
#include <string>
#include <vector>
#include <tuple>

/* Draws the given string at the specified coordinates, line-wrapping using a simple
 * heuristic.
 *
 * This function will *attempt* to stay within the designated width. It will only
 * fail to do so if there is a token that is so wide that it cannot be broken apart.
 *
 * After drawing the text, this function will return a GRectangle containing a
 * bounding box of all the text that was drawn.
 */
GRectangle drawWrappedText(const std::string& text,
                           double x, double y,
                           double width,
                           const std::string& color,
                           const std::string& font,
                           GWindow& window);

/* Returns the bounding rectangle that would be occupied were you to call
 * drawWrappedText with the same set of parameters.
 */
GRectangle measureWrappedText(const std::string& text,
                              double x, double y,
                              double width,
                              const std::string& color,
                              const std::string& font,
                              GWindow& window);

/* Draws wrapped text inside a box, returning the bounding box of the
 * overall text that was written.
 */
GRectangle drawTextBox(const std::string& text,
                       double x, double y,
                       double width,
                       double padding,
                       const std::string& fillColor,
                       const std::string& borderColor,
                       const std::string& fontColor,
                       const std::string& font,
                       GWindow& window);

/* Draws a legend for a chart, returning the bounding box for that legend.
 *
 * The input lists of strings may be shorter than the input list of colors, but must
 * not be longer.
 *
 * If the input list is empty, no legend is drawn and an empty region at (x, y)
 * is returned.
 */
GRectangle drawLegend(const std::vector<std::string>& strings,
                      const std::vector<std::string>& colors,
                      double x, double y, double width,
                      const std::string& textColor,
                      const std::string& font,
                      const std::string& borderColor,
                      GWindow& window);

/* Clears the display, resetting it to a specific color. */
void clearDisplay(GWindow& window, const std::string& backgroundColor);

/* Given a coordinate on the Earth's surface, along with a centering coordinate, returns
 * the (x, y) coordinates at which to display that coordinate in a Mollweide projection.
 * The returned coordinates are in logical space ([-2, +2] x [-1, +1]) and will need to
 * be converted to screen space before being displayed.
 *
 * The input longitudes and latitudes should be in degrees, not radians.
 */
std::tuple<double, double> mollweideProjectionOf(double latitude, double longitude,
                                                 double centerLongitude = 0.0,
                                                 double centerLatitude  = 0.0);

/* Creates a slider bar in the SOUTH console that slides over a given year range. */
Temporary<GSlider> makeYearSlider(GWindow& window, int startYear, int endYear);

/* Given a rectangle, expands that rectangle outward in all directions by delta units.
 * Delta can be negative to contract the box, in which case delta must be smaller than the
 * width and height.
 */
GRectangle expand(const GRectangle& rect, double delta);
GRectangle expand(double x, double y, double width, double height, double delta);

#endif
