#ifndef GUIUtils_Included
#define GUIUtils_Included

#include "TemporaryComponent.h"
#include "gobjects.h"
#include "ginteractors.h"
#include "gwindow.h"
#include <string>
#include <vector>
#include <tuple>
#include <memory>

/**
 * Given a number, returns a version of that number with commas separating the digits.
 *
 * @param number The number in question
 * @return The number with commas added.
 */
std::string addCommasTo(int number);

/* Given a quantity, returns a string of that quantity plus an appropriately-pluralized
 * version of what it is.
 */
template <typename ValueType>
std::string pluralize(const ValueType& value, const std::string& singular, const std::string& plural) {
    return addCommasTo(value) + " " + (value == 1? singular : plural);
}

/* Assume we suffix with s to pluralize unless specified otherwise. */
template <typename ValueType>
std::string pluralize(const ValueType& value, const std::string& singular) {
    return pluralize(value, singular, singular + "s");
}

/* Draws a line of centered text in the given rectangle. */
void drawCenteredText(GWindow& window,
                      const GRectangle& bounds,
                      const std::string& text,
                      const std::string& font,
                      const std::string& color);

/* Type: TextRender
 *
 * Object capable of rendering nicely-formatted text on the screen.
 */
class TextRender {
public:
    /* Bounding rectangle for the text render. */
    GRectangle bounds() const;

    /* Color for this text render. */
    std::string color() const;

    /* Draws this text render in the specified window. */
    void draw(GWindow& window);

    /* Lays out a string to fit within the specified rectangle. The text might not fill up the
     * entirety of that rectangle if it isn't long enough.
     *
     * This function will attempt to respect the font size specified in the font string, but may
     * have to shrink the font size to get things to fit.
     */
    static std::shared_ptr<TextRender> construct(const std::string& text,
                                                 const GRectangle& bounds,
                                                 const std::string& color,
                                                 const std::string& font);

private:
    TextRender() = default;
    GRectangle mBounds;
    GRectangle mComputedBounds;
    std::string mFont;
    std::vector<std::tuple<GPoint, std::string>> mLines;
    std::string mColor;
};

/* Type: LegendRender
 *
 * Object capable of rendering a nicely-formatted chart legend on the screen.
 */
class LegendRender {
public:
    GRectangle bounds() const;

    void draw(GWindow& window);

    /* Draws a legend for a chart, returning the bounding box for that legend.
     *
     * The input lists of strings may be shorter than the input list of colors, but must
     * not be longer.
     */
    static std::shared_ptr<LegendRender> construct(const std::vector<std::string>& strings,
                                                   const std::vector<std::string>& colors,
                                                   const GRectangle& bounds,
                                                   const std::string& textColor,
                                                   const std::string& font,
                                                   const std::string& borderColor);

private:
    LegendRender() = default;
    GRectangle mBounds;
    GRectangle mComputedBounds;
    std::vector<std::shared_ptr<TextRender>> mLines;
    std::vector<std::string> mBulletColors;
    std::string mBorderColor;
};

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
Temporary<GSlider> makeYearSlider(GWindow& window, int startYear, int endYear, int step);

#endif
