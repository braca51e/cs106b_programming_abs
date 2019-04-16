#include "GUIUtils.h"
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

    /* Attempts to line wrap the text to fit it within the given rectangle. On success, returns true and
     * fills in the outparameters with information about the render. Otherwise, returns false and leaves
     * them unmodified.
     */
    bool lineWrapWithin(const vector<string>& tokens,
                        const GRectangle& bounds,
                        const string& font,
                        vector<tuple<GPoint, string>>& lines,
                        GRectangle& computedBounds) {
        /* Set up a rendering label. */
        GText label("");
        label.setFont(font);

        /* Compute the line height from the font metrics. */
        double lineHeight = label.getFontAscent() + label.getFontDescent();

        /* Track the original starting coordinates. We'll need this for later when we compute
         * the bounding box.
         */
        double renderedWidth = 0;
        double renderedHeight = 0;

        /* Initial position is in the upper-left corner, shifted down by the font ascent
         * (since we draw on the baseline.)
         */
        double x = bounds.getX();
        double y = bounds.getY() + label.getFontAscent();

        vector<tuple<GPoint, string>> computedLines;
        string currLine;

        /* Begin laying out tokens. */
        for (const auto& token: tokens) {
            /* If this is newline, insert a hard line break. */
            if (token == "\n") {
                /* Cache the line generated so far. */
                computedLines.push_back(make_tuple(GPoint(bounds.getX(), y), currLine));
                currLine.clear();

                /* Shift down to the next line. */
                x = bounds.getX();
                y += lineHeight * kParagraphSpacing;
            }
            /* Otherwise, if this is another sort of whitespace token, scoot over as appropriate,
             * but don't adjust the bounding rectangle.
             */
            else if (isspace(token[0])) {
                /* Don't do anything if we're at the front of a line. */
                if (int(x - bounds.getX()) != 0) {
                    label.setText(token);
                    currLine += token;
                    x += label.getWidth();
                }
            }
            /* Otherwise, draw this token and update the global bounding rectangle. */
            else {
                label.setText(token);

                /* If we don't fit, advance to the next line. */
                if (x + label.getWidth() > bounds.getX() + bounds.getWidth()) {
                    /* Write out the line we have so far. */
                    computedLines.push_back(make_tuple(GPoint(bounds.getX(), y), currLine));
                    currLine.clear();

                    x = bounds.getX();
                    y += lineHeight * kLineSpacing;
                }

                currLine += token;

                /* Shift over by the width of what we just added. */
                x += label.getWidth();

                /* Update our bounding rectangle. */
                renderedWidth  = max(renderedWidth, x - bounds.getX());
                renderedHeight = max(y + label.getFontDescent() - bounds.getY(), renderedHeight);

                /* If we're out of bounds, report an error. */
                if (renderedWidth > bounds.getWidth() || renderedHeight > bounds.getHeight()) return false;
            }
        }

        /* Process any remaining tokens. */
        if (!currLine.empty()) {
            computedLines.push_back(make_tuple(GPoint(bounds.getX(), y), currLine));
        }

        lines = computedLines;
        computedBounds = { bounds.getX(), bounds.getY(), renderedWidth, renderedHeight };
        return true;
    }

    /* Given a font string, reduces the size of that font by one unit. */
    string reduceFont(const string& font) {
        /* Find the last dash. */
        auto dash = font.rfind('-');
        if (dash == string::npos) error("Font string [" + font + "] missing a dash?");

        /* Extract the existing font size. */
        int fontSize = stringToInteger(font.substr(dash + 1));

        /* If the font size is one or lower, we can't shrink it. */
        if (fontSize <= 1) error("Cannot reduce font size any further!");

        /* Rebuild the font string with the size reduced. */
        return font.substr(0, dash + 1) + to_string(fontSize - 1);
    }
}

shared_ptr<TextRender> TextRender::construct(const string& text,
                                             const GRectangle& bounds,
                                             const string& color,
                                             const string& font) {
    /* Split the text apart into individual tokens. */
    auto tokens = tokenize(text);

    /* Keep trying to get things to fit, shrinking the font until we succeed.
     *
     * TODO: This is a silly O(n)-time algorithm. Use binary search instead?
     */
    shared_ptr<TextRender> result(new TextRender());
    result->mBounds = bounds;
    result->mColor  = color;
    result->mFont   = font;
    while (true) {
        /* See if this works. */
        if (lineWrapWithin(tokens, bounds, result->mFont, result->mLines, result->mComputedBounds)) return result;

        /* Oh fiddlesticks. Back off the font size a bit and try again. */
        result->mFont = reduceFont(result->mFont);
    }
}

void TextRender::draw(GWindow& window) {
    GText label;
    label.setFont(mFont);
    label.setColor(mColor);

    for (const auto& line: mLines) {
        label.setLocation(get<0>(line));
        label.setText(get<1>(line));
        window.draw(label);
    }
}

GRectangle TextRender::bounds() const {
    return mComputedBounds;
}

namespace {
    const double kBulletSize    = 10;
    const double kBulletPadding = 10;
    const double kItemPadding   = 5;
}

shared_ptr<LegendRender> LegendRender::construct(const std::vector<string>& strings,
                                                 const std::vector<string>& colors,
                                                 const GRectangle& bounds,
                                                 const string& textColor,
                                                 const string& font,
                                                 const string& borderColor) {
    /* Validate input. */
    if (strings.size() > colors.size()) error("Not enough colors to draw legend.");

    /* Set up initial fields. */
    shared_ptr<LegendRender> result(new LegendRender());
    result->mBounds = bounds;
    result->mBorderColor = borderColor;
    result->mBulletColors = colors;

    /* Attempt to use the current font size to make everything work. If it does work, great! If not, back off
     * the font size and try again.
     *
     * TODO: This uses a silly O(n)-time algorithm. Switch to binary search?
     */
    string currFont = font;
    while (true) {
        /* Current x and y coordinates, for the purposes of laying out text. */
        double x = bounds.getX();
        double y = bounds.getY() + kItemPadding;

        vector<shared_ptr<TextRender>> entries;

        /* TODO: Short-circuit if we fail early? */
        const double bulletSpacing = kBulletSize + 2 * kBulletPadding;
        double width = 0;
        for (int i = 0; i < strings.size(); i++) {
            /* Construct a text render for this item, tracking where it ended up. */


            /* Give unbounded vertical space. We'll do adjustments for height later on. */
            auto render = TextRender::construct(strings[i], {
                                                    x + bulletSpacing, y,
                                                    bounds.getWidth() - bulletSpacing - kItemPadding,
                                                    numeric_limits<double>::max()
                                                }, textColor, currFont);

            y = render->bounds().getY() + render->bounds().getHeight() + kItemPadding;
            entries.push_back(render);

            width = max(width, render->bounds().getWidth());
        }

        /* Get the net bounding box. We'll force this to be as wide as the space that we're given
         * for consistency's sake.
         */
        GRectangle computedBounds = {
            bounds.getX(), bounds.getY(), bounds.getWidth(), y - bounds.getY()
        };

        if (computedBounds.getHeight() <= bounds.getHeight()) {
            result->mComputedBounds = computedBounds;
            result->mLines = entries;
            return result;
        }

        /* Oops, didn't fit. Shrink the font and try again. */
        currFont = reduceFont(currFont);
    }
}

void LegendRender::draw(GWindow& window) {
    /* First, draw all the text items and their corresponding bullets. */
    for (int i = 0; i < mLines.size(); i++) {
        mLines[i]->draw(window);

        /* Draw the bullet, vertically-centered. */
        window.setColor(mBulletColors[i]);
        window.fillRect(mComputedBounds.getX() + kBulletPadding,
                        mLines[i]->bounds().getY() + mLines[i]->bounds().getHeight() / 2.0 - kBulletSize / 2.0,
                        kBulletSize, kBulletSize);
    }

    /* Draw the overall bounding box. */
    window.setColor(mBorderColor);
    window.drawRect(mComputedBounds);
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

Temporary<GSlider> makeYearSlider(GWindow& window, int startYear, int endYear, int step) {
    Temporary<GSlider> result(new GSlider(0, (endYear - startYear) / step + 1, 0),
                              window,
                              "SOUTH");
    result->setPaintTicks(true);
    result->setSnapToTicks(true);
    result->setBounds(0, 0, window.getCanvasWidth(), result->getHeight());
    return result;
}

void drawCenteredText(GWindow& window, const GRectangle& bounds,
                      const string& text, const string& font, const string& color) {
    GText label(text);
    label.setFont(font);
    label.setColor(color);

    double x = bounds.getX() + (bounds.getWidth()  - label.getWidth())      / 2.0;
    double y = bounds.getY() + (bounds.getHeight() + label.getFontAscent()) / 2.0;

    window.draw(&label, x, y);
}

/* Inserts commas into a numeric string. */
string addCommasTo(int val) {
    string asStr = to_string(val);
    const int length = int(asStr.length());

    string result;
    for (int i = 0; i < length; i++) {
        /* Run backwards through the source string so determining where commas go
         * becomes a lot easier.
         */
        result = asStr[length - 1 - i] + result;

        /* Put commas in provided we've already added three characters, but aren't
         * about to use all the digits up.
         */
        if (i % 3 == 2 && i < length - 1) {
            result = ',' + result;
        }
    }
    return result;
}
