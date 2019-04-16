#include "Demos/GUIUtils.h"
#include "strlib.h"
#include <sstream>
#include <vector>
#include <cmath>
using namespace std;

namespace {
    /* Multiplier of the line height to use between natural linebreaks. */
    const double kLineSpacing = 1.1;

    /* Multiplier of the line height to use between paragraph breaks. */
    const double kParagraphSpacing = kLineSpacing + 0.35;

    /* Tokenizes a string into individually-renderable units. */
    vector<string> tokenize(const string& text) {
        istringstream input(text);

        string current;
        vector<string> result;

        while (true) {
            int next = input.get();

            /* If we hit EOF, flush the current token and stop. */
            if (next == EOF) {
                if (!current.empty()) result.push_back(current);
                return result;
            }

            /* If we hit a space, flush the current token and start a new one. */
            if (isspace(char(next))) {
                if (!current.empty()) result.push_back(current);
                current = string(1, char(next));
            }
            /* Otherwise, we have a non-space character. Extend the current token, flushing
             * it if it's a whitespace token.
             */
            else {
                if (!current.empty() && isspace(current[0])) {
                    result.push_back(current);
                    current.clear();
                }

                current += char(next);
            }
        }
    }

    /* Wraps the given text in the given window. If the last parameter is true, the text
     * is drawn. Otherwise, it isn't drawn.
     */
    GRectangle doWrappedText(const string& text,
                             double x, double y,
                             double width,
                             const string& color,
                             const string& font,
                             GWindow& window,
                             bool render) {
        /* Split the text apart into individual tokens. */
        auto tokens = tokenize(text);

        /* Set up a rendering label. */
        GText label("");
        label.setColor(color);
        label.setFont(font);

        /* Compute the line height from the font metrics. */
        double lineHeight = label.getFontAscent() + label.getFontDescent();

        /* Track the original starting coordinates. We'll need this for later when we compute
         * the bounding box.
         */
        double x0 = x, y0 = y;
        double renderedWidth = 0;
        double renderedHeight = 0;

        /* Initial position is in the upper-left corner, shifted down by the font ascent
         * (since we draw on the baseline.)
         */
        y += label.getFontAscent();

        /* Begin laying out tokens. */
        for (const auto& token: tokens) {
            /* If this is newline, insert a hard line break. */
            if (token == "\n") {
                x = x0;
                y += lineHeight * kParagraphSpacing;
            }
            /* Otherwise, if this is another sort of whitespace token, scoot over as appropriate,
             * but don't adjust the bounding rectangle.
             */
            else if (isspace(token[0])) {
                /* Don't do anything if we're at the front of a line. */
                if (int(x - x0) != 0) {
                    label.setText(token);
                    x += label.getWidth();
                }
            }
            /* Otherwise, draw this token and update the global bounding rectangle. */
            else {
                label.setText(token);

                /* If we don't fit, advance to the next line. */
                if (x + label.getWidth() > x0 + width) {
                    x = x0;
                    y += lineHeight * kLineSpacing;
                }

                /* Render the token. */
                if (render) window.draw(&label, x, y);
                x += label.getWidth();

                /* Update our bounding rectangle. */
                renderedWidth  = max(renderedWidth, x - x0);
                renderedHeight = max(y + label.getFontDescent() - y0, renderedHeight);
            }
        }

        return { x0, y0, renderedWidth, renderedHeight };
    }
}

GRectangle drawWrappedText(const string& text,
                           double x, double y,
                           double width,
                           const string& color,
                           const string& font,
                           GWindow& window) {
    return doWrappedText(text, x, y,  width, color, font, window, true);
}

GRectangle measureWrappedText(const string& text,
                              double x, double y,
                              double width,
                              const string& color,
                              const string& font,
                              GWindow& window) {
    return doWrappedText(text, x, y,  width, color, font, window, false);
}

GRectangle drawTextBox(const string& text,
                       double x, double y,
                       double width,
                       double padding,
                       const string& fillColor,
                       const string& borderColor,
                       const string& fontColor,
                       const string& font,
                       GWindow& window) {
    auto textBounds = measureWrappedText(text,
                                         x + padding, y + padding,
                                         width - 2 * padding,
                                         fontColor,
                                         font, window);

    /* Inflate those bounds. */
    GRectangle result = {
        textBounds.getX() - padding, textBounds.getY() - padding,
        textBounds.getWidth() + 2 * padding, textBounds.getHeight() + 2 * padding
    };

    /* Draw the box. */
    window.setColor(fillColor);
    window.fillRect(result);

    window.setColor(borderColor);
    window.drawRect(result);

    /* Draw the text. */
    drawWrappedText(text,
                    x + padding, y + padding,
                    width - 2 * padding,
                    fontColor,
                    font, window);
    return result;

}

namespace {
    const double kBulletSize    = 10;
    const double kBulletPadding = 10;
    const double kItemPadding   = 5;
}

GRectangle drawLegend(const vector<string>& strings,
                      const vector<string>& colors,
                      double x0, double y0, double width,
                      const string& textColor,
                      const string& font,
                      const string& borderColor,
                      GWindow& window) {
    /* Validate input. */
    if (strings.size() > colors.size()) error("Not enough colors to draw legend.");

    /* If nothing is selected, there's nothing to draw. */
    if (strings.empty()) return { x0, y0, 0, 0 };

    /* Current x and y coordinates, for the purposes of laying out text. */
    double x = x0;
    double y = y0 + kItemPadding;

    for (int i = 0; i < strings.size(); i++) {
        /* Draw the text for this country, tracking where it ended up. */
        double bulletSpacing = kBulletSize + 2 * kBulletPadding;
        auto bounds = drawWrappedText(strings[i], x + bulletSpacing, y,
                                      width - bulletSpacing,
                                      textColor,
                                      font, window);

        /* Draw the bullet, vertically-centered. */
        window.setColor(colors[i]);
        window.fillRect(x + kBulletPadding,
                        bounds.getY() + bounds.getHeight() / 2.0 - kBulletSize / 2.0,
                        kBulletSize, kBulletSize);

        y = bounds.getY() + bounds.getHeight() + kItemPadding;

    }

    /* Draw the bounding box. */
    GRectangle result = {
        x0, y0, width, y - y0
    };
    window.setColor(borderColor);
    window.drawRect(result);
    return result;
}

void clearDisplay(GWindow& window, const std::string& backgroundColor) {
    window.setColor(backgroundColor);
    window.fillRect(0, 0, window.getCanvasWidth(), window.getCanvasHeight());
}

/* The math behind this function is shamelessly lifted (with love) from Wikipedia:
 *
 *   https://en.wikipedia.org/wiki/Mollweide_projection#Mathematical_formulation
 *
 * The inverse formula is way easier than the forward formula. :-)
 *
 * The output of this function is a pair of real numbers in the range [-2, +2] x [-1, 1],
 * which then needs to be scaled and translated based on the window size.
 */
tuple<double, double> mollweideProjectionOf(double latitude, double longitude,
                                            double longitudeOffset, double latitudeOffset) {
    /* Adjust longitude to fit map. */
    longitude -= longitudeOffset;
    if (longitude < -180) longitude += 360;
    if (longitude > 180)  longitude -= 360;

    latitude -= latitudeOffset;
    if (latitude < -90)   latitude += 180;
    if (latitude >  90)   latitude  -= 180;

    /* Convert from degrees (what we get back from USGS) to radians. */
    longitude *= M_PI / 180;
    latitude  *= M_PI / 180;

    /* There isn't a closed-form solution to work out the coordinates, so we'll
     * use Newton's method to try to get close to one. Thanks, calculus!
     */
    static const size_t kNumIterations = 100;
    double theta = latitude;
    for (size_t i = 0; i < kNumIterations; i++) {
        theta = theta - (2 * theta + sin(2 * theta) - M_PI * sin(latitude)) / (2 + 2 * cos(2 * theta));
    }

    /* Armed with theta, we can work out the x and y coordinates. */
    double x = 2 * cos(theta) * longitude / M_PI;
    double y = sin(theta);

    return make_tuple(x, y);
}

Temporary<GSlider> makeYearSlider(GWindow& window, int startYear, int endYear) {
    Temporary<GSlider> result(new GSlider(startYear, endYear, startYear),
                              window,
                              "SOUTH");
    result->setPaintLabels(true);
    result->setPaintTicks(true);
    result->setSnapToTicks(true);
    result->setMajorTickSpacing(10);
    result->setMinorTickSpacing(1);
    result->setBounds(0, 0, window.getCanvasWidth(), result->getHeight());
    return result;
}

GRectangle expand(const GRectangle& rect, double delta) {
    return {
        rect.getX() - delta, rect.getY() - delta,
        rect.getWidth() + 2 * delta, rect.getHeight() + 2 * delta
    };
}
GRectangle expand(double x, double y, double width, double height, double delta) {
    return expand({ x, y, width, height }, delta);
}
