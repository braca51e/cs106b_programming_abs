#include "Demos/RidingCircuitGUI.h"
#include "Demos/GUIUtils.h"
#include "Demos/GVector.h"
#include "Utilities/CSV.h"
#include "RidingCircuit.h"
#include "filelib.h"
#include "goptionpane.h"
#include "gfilechooser.h"
#include "pqueue.h"
#include "hashset.h"
#include <algorithm>
#include <limits>
using namespace std;

namespace {
    const string kBackgroundColor = "#FFFFFF";

    /* Padding from the window to the content area. */
    const double kWindowPadding = 50;

    /* Animation parameters. */
    const double kFramesPerSecond = 48;
    const double kPauseTime       = 1000.0 / kFramesPerSecond;

    const double kSecondsPerPoint = 1;
    const int    kFramesPerPoint  = static_cast<int>(kSecondsPerPoint * kFramesPerSecond);

    const string kPointColor       = "Black";
    const string kLineColor        = "Blue";
    const string kActiveLineColor  = "Red";
    const double kLineWidth        = 3;

    const string kPointFillColor   = "#000080";
    const string kPointBorderColor = "#000040";
    const double kPointRadius      = 10;

    /* Amount to expand the bounding box of all the points by to avoid zero divides. */
    const double kExpansionEpsilon = 0.1;

    /* Creates a GOval representing one of the points on a path. */
    shared_ptr<GOval> makePoint(const GPoint& pt) {
        auto oval = make_shared<GOval>(pt.getX() - kPointRadius,
                                       pt.getY() - kPointRadius,
                                       2 * kPointRadius,
                                       2 * kPointRadius);
        oval->setFilled(true);
        oval->setColor(kPointBorderColor);
        oval->setFillColor(kPointFillColor);
        return oval;
    }

    /* Given a filename, loads all the points from that file and returns them in the
     * order in which they appear.
     */
    Vector<Point> loadPointsFrom(const string& filename) {
        CSV data = CSV::parseFile(filename);

        Vector<Point> result;
        for (int row = 0; row < data.numRows(); row++) {
            result.add({ stringToInteger(data[row][0]), stringToInteger(data[row][1]) });
        }

        return result;
    }

    /* Returns a bounding box containing all the points in a particular collection. */
    tuple<int, int, int, int> boundingBoxFor(const Vector<Point>& points) {
        /* Edge case: if there aren't any points, return an empty box. */
        if (points.isEmpty()) return make_tuple(0, 0, 0, 0);

        /* Otherwise, we have a collection of points that we need to bound. */
        int lowX  = numeric_limits<int>::max(),
            lowY  = numeric_limits<int>::max(),
            highX = numeric_limits<int>::min(),
            highY = numeric_limits<int>::min();

        for (auto pt: points) {
            lowX  = min(lowX,  pt.getX());
            lowY  = min(lowY,  pt.getY());
            highX = max(highX, pt.getX());
            highY = max(highY, pt.getY());
        }

        return make_tuple(lowX, lowY, highX, highY);
    }

    /* Smoothstep interpolation. */
    double interpolate(double alpha) {
        return 3 * alpha * alpha - 2 * alpha * alpha * alpha;
    }

    /* Constants defining the strain associated with a given change in direction. */
    const int kStrainForSame    = 0;
    const int kStrainForTurn    = 1;
    const int kStrainForReverse = 5; // We really, really don't want to have to do this.

    /* Type representing an axis-aligned heading. */
    struct Heading {
        Heading(int dx = 1, int dy = 0) {
            if (dx == 0 && dy == 0) error("Internal error: zero vector?");
            if (dx != 0 && dy != 0) error("Internal error: Heading not axis-aligned?");

            x = (dx == 0? dx : dx / abs(dx));
            y = (dy == 0? dy : dy / abs(dy));
        }

        int x, y;
    };

    bool operator== (const Heading& lhs, const Heading& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
    int hashCode(const Heading& h) {
        return ::hashCode(h.x, h.y);
    }
    int dotProductOf(const Heading& lhs, const Heading& rhs) {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    }

    /* Given two headings, returns the "strain" between those headings. */
    int strainFor(const Heading& one, const Heading& two) {
        int dot = dotProductOf(one, two);

        /* This value is 1 if we're aligned, 0 if we rotated 90 degrees, and -1 if we
         * reversed direction.
         */
        if      (dot ==  1) return kStrainForSame;
        else if (dot ==  0) return kStrainForTurn;
        else if (dot == -1) return kStrainForReverse;
        else throw ErrorException("Internal error: dot product should be -1, 0, or +1.");
    }

    /* Given two points, returns all possible directions we could move to go between
     * those two points. If they're collinear, this is just "move in a straight line";
     * if not, it'll be "move in one of the two possible directions that make progress."
     */
    Vector<Heading> headingsBetween(const Point& from, const Point& to) {
        int dx = to.getX() - from.getX();
        int dy = to.getY() - from.getY();

        /* If exactly one component of the heading is zero, that's our only option. */
        if (dx == 0 && dy == 0) error("Internal error: coincident points");
        if (dx == 0 || dy == 0) return { { dx, dy } };

        /* Otherwise, each of the x and y vectors are possible. */
        return { { dx, 0 }, { 0, dy } };
    }

    /* Type representing a pairing of a node and a heading, for use in a HashMap. */
    struct DirectedPoint {
        int index;        // Which point index this is.
        Heading heading;
    };
    bool operator== (const DirectedPoint& lhs, const DirectedPoint& rhs) {
        return lhs.index == rhs.index && lhs.heading == rhs.heading;
    }
    int hashCode(const DirectedPoint& dp) {
        return ::hashCode(dp.index, dp.heading);
    }

    /* Type representing a pairing of a heading and a directed point. */
    struct RouteKey {
        DirectedPoint point;
        Heading       initial;
    };
    bool operator == (const RouteKey& lhs, const RouteKey& rhs) {
        return lhs.point == rhs.point && lhs.initial == rhs.initial;
    }
    int hashCode(const RouteKey& key) {
        return ::hashCode(key.point, key.initial);
    }

    /* Given a list of Points representing a path, deduplicates the nodes in that path
     * so that no two equal nodes are adjacent.
     */
    Vector<Point> deduplicate(const Vector<Point>& points) {
        Vector<Point> result;
        for (auto pt: points) {
            if (result.isEmpty() || result.back() != pt) result += pt;
        }
        return result;
    }

    /* Given a route consisting of a bunch of points annotated with the direction to
     * move at those points, expands that into an actual route consisting of the set
     * of points to visit along the way. For straight-line paths, there isn't anything
     * to do. For paths that involve a 90 degree turn, this involves introducing new
     * midpoints.
     */
    Vector<Point> toWaypoints(const Vector<Point>& points,
                              const Vector<DirectedPoint>& path) {
        Vector<Point> result;

        /* Add the start point (fencepost problem). */
        result.add(points[path[0].index]);

        for (int i = 0; i < points.size(); i++) {
            /* Get the true direction vector from this point to the next. */
            Point from = points[i];
            Point to   = points[(i + 1) % points.size()];
            int dx = to.getX() - from.getX();
            int dy = to.getY() - from.getY();

            /* Project this onto the heading vector. The formula for this projection is
             *
             *   proj_H D = (D . H) H / ||H||
             *
             * and since H is a unit vector, this is
             *
             *   proj_H D = (D . H) H
             *
             * which is easy to compute.
             */
            int dot = path[i].heading.x * dx + path[i].heading.y * dy;
            Point midpoint = {
                from.getX() + dot * path[i].heading.x,
                from.getY() + dot * path[i].heading.y
            };

            /* We always add the midpoint. It's also possible that the midpoint happens
             * to be the destination. If not, then we'll also add the destination.
             */
            result += midpoint;
            if (midpoint != to) result += to;
        }

        return result;
    }

    /* Given a route (represented as a series of logical points to visit), converts
     * it into a list of waypoints that need to be visited in a graphical tour of
     * those points.
     *
     * Given two points, either (1) they're cardinally colinear (they share an x or
     * y coordinate), in whih case there's a pretty clear way to show how to get from
     * one point to the next (just go in a straight line), or (2) they aren't. In
     * that latter case, we have two options of how to break the line apart into
     * straight segments, depending on whether we go horizontally or vertically first.
     *
     * This ends up being a graph search problem. Each node represents a pair of a
     * location and an orientation (up, down, left, right). The edges have costs
     * associated with them (0 for "keep going the way you're going", some constant
     * for "rotate 90 degrees," and some large constant for "reverse your direction."
     * The cost of the very last edge is path-dependent and factors in what our
     * initial direction was.
     */
    Vector<Point> routeToWaypoints(const Vector<Point>& points) {
        /* Begin by deduplicating the points list so that we don't have two copies of the
         * same point in a row.
         */
        auto uniquePts = deduplicate(points);

        /* Edge case: If there are zero or one points, we're done. */
        if (uniquePts.size() == 0) return { };
        if (uniquePts.size() == 1) return { points[0] };

        /* Seed the worklist with each possible first direction we can go. That first
         * direction costs us nothing.
         */
        PriorityQueue<Vector<DirectedPoint>> worklist;
        for (auto dir: headingsBetween(uniquePts[0], uniquePts[1])) {
            worklist.enqueue({ { 0, dir } }, 0);
        }

        /* These paths have a weird fencepost problem to work through - the number of
         * headings should be one less than the number of points. We will encode things
         * in the following way: each point, except the last, has a valid heading. The
         * last point's heading is the direction of motion that was last used at that
         * point. For example, if we went up from A to B, then right from B to C, the
         * path would look like (A, up), (B, right), (C, right), with the last right
         * meaning "that's the direction we are going at the moment." It will be overwritten
         * with the direction that was actually picked for that point when the path is
         * expanded.
         */

        /* To keep track of where we've been, we need to track both the endpoint of the path
         * (where we are) as well as the heading that we used when we started. Two paths that
         * end at the same point might actually be different paths in the larger "metagraph"
         * because the cost of the last edge from the final point back to the start factors
         * in the strain between the last node and the first.
         */
        HashSet<RouteKey> used;

        while (!worklist.isEmpty()) {
            double cost = worklist.peekPriority();
            auto   path = worklist.dequeue();

            /* If this path ends up at the target, we're done. We can tell that we're
             * back home because the length of the path would be exactly the number of
             * points, plus one (because we return where we started.)
             */
            if (path.size() == uniquePts.size() + 1) return toWaypoints(uniquePts, path);

            /* If we've already visited this point from this direction (and with the same
             * starting heading), skip it.
             */
            if (used.contains({ path.back(), path.front().heading })) continue;
            used.add({ path.back(), path.front().heading });

            /* Otherwise, we aren't done. Get all the possible next headings. */
            int curr = path.back().index;
            int next = (curr + 1) % uniquePts.size();
            auto headings = headingsBetween(uniquePts[curr], uniquePts[next]);
            for (int i = 0; i < headings.size(); i++) {
                auto nextPath = path;

                /* Overwrite the (dummy) heading of the last point in the path with
                 * the chosen heading.
                 */
                nextPath.back().heading = headings[i];

                /* There are two options here.
                 *
                 * 1. There's exactly one choice of which direction to go. In that case,
                 *    we have to move in that direction, so we need to set the last point's
                 *    direction to that direction.
                 */
                if (headings.size() == 1) {
                    nextPath.add({ next, headings[i] });
                }
                /* 2. There are two choices. In that case, whichever heading we pick to
                 *    move in, the direction on entry to the newly-added point will be
                 *    the other one. For example, if we can move both up and right, then
                 *    if we pick "up" as our direction, we enter the next point from the
                 *    direction "right."
                 */
                else if (headings.size() == 2) {
                    nextPath.add({ next, headings[!i]});
                } else {
                    error("Internal error: Number of headings should be either 1 or 2.");
                }

                /* The cost of choosing this option corresponds to the strain between the
                 * direction we were following (path.back().heading, and NOT the (overwritten)
                 * value of nextPath.back().heading) and the heading we chose to follow.
                 */
                double nextCost = cost + strainFor(path.back().heading, headings[i]);

                /* However, when we close the cycle, we have to factor in the strain between
                 * the direction we took re-entering the start node and the direction we chose
                 * when we left the start node.
                 */
                if (next == 0) {
                    nextCost += strainFor(nextPath.front().heading, nextPath.back().heading);
                }

                worklist.enqueue(nextPath, nextCost);
            }
        }

        /* This should be unreachable - it would only happen if there was no possible path
         * through the points.
         */
        throw ErrorException("Internal error: no path was found?");
    }

    /* Given a list of waypoints, returns the total length of all the lines in the
     * route induced by those waypoints.
     */
    int lengthOf(const Vector<Point>& points) {
        int total = 0;

        for (int i = 0; i < points.size() - 1; i++) {
            total += distanceBetween(points[i], points[i + 1]);
        }

        return total;
    }
}

struct RidingCircuitGUI::Impl {
    /* The currently-selected collection of points. */
    Vector<Point> mPoints;

    /* The route we're going to animate, represented as a list of the waypoints
     * that comprise it, in graphics space.
     */
    Vector<GPoint> mWaypoints;

    /* Bounding rectangle for all graphics. */
    GRectangle mBounds;

    /* List of all graphics objects that need to be displayed, in the order in
     * which they should be displayed. We use this to update the graphics
     * state at each point in time.
     */
    Deque<std::shared_ptr<GObject>> mGraphicsObjects;

    /* Convenient little package for holding all the information necessary to
     * map from logical space to window space.
     */
    struct Graphics {
        GRectangle bounds;  // Bounding rectangle where everything should be drawn.
        int minX, minY;     // Lowest X and Y values in integer space.
        double scale;       // Scaling factor: how much one integer unit translates into graphics
    };
    Graphics mGraphics;

    /* Current animation status. */
    int  mFrame = 0;              // Which animation frame this is.
    int  mMaxFrame;               // Highest frame number in the animation.
    bool mAnimationActive = true; // Whether the animation is running. This is designed to prevent
                                  // spurious draws if we get timer events faster than we can
                                  // handle them.

    /* The GLines that make up the current segment. */
    Vector<std::shared_ptr<GLine>> mCurrentLines;

    /* Timer to tick off the animation. We use a shared_ptr here so that
     * if we start and stop several different animations, we can keep track
     * of which time is associated with the current one.
     */
    std::shared_ptr<GTimer> mTimer;

    /* Controls. */
    Temporary<GButton> mLoadButton;
    Temporary<GButton> mSolveButton;

    /* Status line. */
    Temporary<GLabel> mStatusLine;

    /* Pointer back to the enclosing object. */
    RidingCircuitGUI* mOwner;

    /* Advances the animation forward by a single step. */
    void stepAnimation();

    /* Converts from integer space to screen space using the current graphics system. */
    GPoint toGraphics(const Point& pt);

    /* Computes the graphics system necessary to display the specified set of points. */
    void computeGraphics();

    /* Adds all the points in the specified list to the display. */
    void drawPoints(const Vector<Point>& points);

    /* Clears all current objects. */
    void clearObjects();
};

RidingCircuitGUI::RidingCircuitGUI(GWindow& window) {
    mImpl = make_shared<Impl>();
    mImpl->mOwner = this;

    mImpl->mLoadButton  = Temporary<GButton>(new GButton("Load Points"), window, "EAST");
    mImpl->mSolveButton = Temporary<GButton>(new GButton("Solve!"),      window, "EAST");
    mImpl->mStatusLine  = Temporary<GLabel> (new GLabel(" "),            window, "SOUTH");

    mImpl->mBounds = expand(0, 0, window.getCanvasWidth(), window.getCanvasHeight(), -kWindowPadding);
}

/* We need to clean up our messes and remove from the window all the objects
 * we added in.
 */
RidingCircuitGUI::~RidingCircuitGUI() {
    mImpl->clearObjects();
}

string RidingCircuitGUI::name() {
    return "Riding Circuit";
}

void RidingCircuitGUI::timerFired() {
    if (mImpl->mAnimationActive) {
        mImpl->stepAnimation();
    }
}

void RidingCircuitGUI::actionPerformed(GObservable* source) {
    if (source == mImpl->mLoadButton) {
        string filename = GFileChooser::showOpenDialog("Choose Points", ".", "CSV Files (*.csv)");
        if (filename.empty()) return;
        if (!endsWith(filename, ".csv")) {
            GOptionPane::showMessageDialog("Circuit files should have the .csv extension.");
            return;
        }

        mImpl->mPoints = loadPointsFrom(filename);

        /* Reset graphics and animation state. */
        mImpl->mTimer.reset();
        mImpl->mAnimationActive = false;
        mImpl->clearObjects();
        mImpl->computeGraphics();
        mImpl->drawPoints(mImpl->mPoints);
        mImpl->mStatusLine->setText(" ");  // So as not to carry over older solution values

        requestRepaint();
    }
    else if (source == mImpl->mSolveButton) {
        /* Reset graphics and animation state. */
        mImpl->clearObjects();
        mImpl->mWaypoints.clear();
        mImpl->drawPoints(mImpl->mPoints);

        /* Compute the route to use. */
        mImpl->mSolveButton->setEnabled(false);
        mImpl->mLoadButton->setEnabled(false);
        mImpl->mStatusLine->setText("Solving...");
        auto points = bestCircuitThrough(mImpl->mPoints);
        mImpl->mStatusLine->setText("Routing...");
        auto waypoints = routeToWaypoints(points);
        mImpl->mStatusLine->setText("Optimal route has length " + to_string(lengthOf(waypoints)));
        mImpl->mSolveButton->setEnabled(true);
        mImpl->mLoadButton->setEnabled(true);

        /* Convert the route into a series of GPoints. */
        for (const auto& pt: waypoints) {
            mImpl->mWaypoints += mImpl->toGraphics(pt);
        }

        /* Run the animation! */
        mImpl->mFrame = 0;
        mImpl->mMaxFrame = 1 + kFramesPerPoint * mImpl->mPoints.size(); // 1+ to avoid division by zero errors.
        mImpl->mAnimationActive = true;

        mImpl->mTimer = make_shared<GTimer>(kPauseTime);
        mImpl->mTimer->start();
    }
}

void RidingCircuitGUI::repaint(GWindow& window) {
    clearDisplay(window, kBackgroundColor);

    /* Remove and redraw all graphics objects. */
    for (auto gobj: mImpl->mGraphicsObjects) {
        window.add(gobj.get());
    }
}

void RidingCircuitGUI::Impl::stepAnimation() {
    /* If this is frame 0, we need to configure the lines for this stretch of the
     * animation.
     */
    if (mFrame == 0) {
        /* Assemble the GLines to use and add them, hidden. We won't actually reposition
         * them until we have some progress.
         */
        mCurrentLines.clear();
        for (int i = 0; i < mWaypoints.size() - 1; i++) {
            /* Each line's start point can be fixed now, but its endpoint will be
             * adjusted periodically through the animation and so we don't need to
             * anchor it now.
             */
            auto line = make_shared<GLine>(mWaypoints[i], GPoint{ 0, 0 });
            line->setLineWidth(kLineWidth);
            line->setColor(kLineColor);
            line->setVisible(false);

            /* Place this at the front so that it underdraws all other objects. */
            mGraphicsObjects.addFront(line);

            mCurrentLines += line;
        }
    }

    /* Step the animation. */
    mFrame++;

    /* Interpolate our progress. */
    double alpha = interpolate(mFrame / double(mMaxFrame));

    /* Compute the total (Manhattan) distance this route covers. */
    double totalDistance = 0;
    for (int i = 0; i < mWaypoints.size() - 1; i++) {
        auto v = mWaypoints[i + 1] - mWaypoints[i];

        totalDistance += fabs(v.x) + fabs(v.y);
    }

    double distance = 0;
    for (int i = 0; i < mWaypoints.size() - 1; i++) {
        /* Use Manhattan distance. */
        auto v = mWaypoints[i + 1] - mWaypoints[i];
        double ourLength = fabs(v.x) + fabs(v.y);

        /* If this point starts purely before alpha, then there's nothing to do for
         * this point or any later ones.
         */
        if (distance / totalDistance > alpha) {
            break;
        }

        /* If alpha exceeds this distance fraction, connect this line to its destination
         * and make it visible.
         */
        if ((distance + ourLength) / totalDistance < alpha) {
            auto endpoint = mWaypoints[i + 1];
            mCurrentLines[i]->setEndPoint(endpoint.getX(), endpoint.getY());
            mCurrentLines[i]->setVisible(true);
        }
        /* Otherwise, alpha is somewhere in the middle of this segment. Draw as much
         * of this segment as is needed.
         */
        else {
            double progress = (alpha - (distance / totalDistance)) / (ourLength / totalDistance);
            auto endpoint   = mWaypoints[i] + progress * v;
            mCurrentLines[i]->setEndPoint(endpoint.getX(), endpoint.getY());
            mCurrentLines[i]->setVisible(true);
        }

        /* Either way, make some progress. */
        distance += ourLength;
    }

    /* Wrap up the animation, if necessary. */
    if (mFrame >= mMaxFrame) {
        mAnimationActive = false;
        mTimer.reset();
    }
    mOwner->requestRepaint();
}

void RidingCircuitGUI::Impl::computeGraphics() {
    Graphics result;

    /* Fill in the raw bounding box in integer space. */
    int maxX, maxY;
    tie(result.minX, result.minY, maxX, maxY) = boundingBoxFor(mPoints);

    /* Compute a graphics bounding box into which to place the points. Begin by
         * expanding the true bounding box by some small epsilon, which helps avoid
         * dividing by zero in the case where one of the box dimensions is zero.
         */
    auto rawBounds = expand(result.minX, result.minY,
                            maxX - result.minX,
                            maxY - result.minY,
                            kExpansionEpsilon);

    double xScale = mBounds.getWidth()  / rawBounds.getWidth();
    double yScale = mBounds.getHeight() / rawBounds.getHeight();
    result.scale = min(xScale, yScale);

    /* Construct the actual bounding box. */
    double width  = rawBounds.getWidth()  * result.scale;
    double height = rawBounds.getHeight() * result.scale;
    result.bounds = {
        mBounds.getX() + (mBounds.getWidth()  - width)  / 2,
        mBounds.getY() + (mBounds.getHeight() - height) / 2,
        width, height
    };

    mGraphics = result;
}

GPoint RidingCircuitGUI::Impl::toGraphics(const Point& pt) {
    double x = mGraphics.bounds.getX() + (pt.getX() - mGraphics.minX) * mGraphics.scale;
    double y = mGraphics.bounds.getY() + (pt.getY() - mGraphics.minY) * mGraphics.scale;
    return { x, y };
}

void RidingCircuitGUI::Impl::drawPoints(const Vector<Point>& points) {
    /* Create GOvals for each of the points. */
    for (auto pt: points) {
        mGraphicsObjects.addBack(makePoint(toGraphics(pt)));
    }
}

void RidingCircuitGUI::Impl::clearObjects() {
    for (auto gobj: mGraphicsObjects) {
        auto parent = gobj->getParent();
        if (parent) gobj->getParent()->remove(gobj.get());
    }
    mGraphicsObjects.clear();
    mCurrentLines.clear();
}
