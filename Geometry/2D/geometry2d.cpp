#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
typedef long double ld;

// ===================== PICK THE NUMBER TYPE (change only these 2 lines) =====================
typedef long long T;   const ld EPS = 0;        // integer input -> everything exact
// typedef long double T; const ld EPS = 1e-9;  // real input, or you need intersection points
//
// * Every comparison goes through sign(), so the SAME code is exact for ll and eps-safe for ld.
//   Never write a == b / a < b on coordinates yourself -> write sign(a - b) == 0 / sign(a - b) < 0.
// * Functions marked [D] produce non-integer points -> they need T = long double.
//   Functions returning ld (distances, angles, areas) work fine with T = long long.
// * ll overflow: cross/dot are quadratic -> coords up to ~1e9 are ok (<= ~4e18).
// * EPS: 1e-9 suits coords up to ~1e4..1e5. For huge coords use something like 1e-7.
// ============================================================================================

const ld PI = acosl(-1);

// -1 if x < 0, 0 if x == 0, +1 if x > 0 (with EPS)
int sign(ld x) { return (x > EPS) - (x < -EPS); }

struct Point {
    T x, y;
    Point(T x = 0, T y = 0) : x(x), y(y) {}
    Point operator+(Point p) const { return Point(x + p.x, y + p.y); }
    Point operator-(Point p) const { return Point(x - p.x, y - p.y); }
    Point operator*(T k) const { return Point(x * k, y * k); }
    Point operator/(T k) const { return Point(x / k, y / k); }
    bool operator==(Point p) const { return sign(x - p.x) == 0 && sign(y - p.y) == 0; }
    bool operator!=(Point p) const { return !(*this == p); }
    bool operator<(Point p) const { return sign(x - p.x) != 0 ? x < p.x : sign(y - p.y) < 0; }
};
istream& operator>>(istream& in, Point& p) { return in >> p.x >> p.y; }
ostream& operator<<(ostream& out, Point p) { return out << p.x << " " << p.y; }


// ======================================= BASICS =======================================

T dot(Point a, Point b) { return a.x * b.x + a.y * b.y; }
T cross(Point a, Point b) { return a.x * b.y - a.y * b.x; }
T cross(Point o, Point a, Point b) { return cross(a - o, b - o); }   // = 2 * signed area of triangle oab
T norm2(Point a) { return a.x * a.x + a.y * a.y; }                    // |a|^2
T dist2(Point a, Point b) { return norm2(a - b); }                     // |a-b|^2
ld len(Point a) { return sqrtl((ld)a.x * a.x + (ld)a.y * a.y); }     // |a|
ld dist(Point a, Point b) { return len(a - b); }

// +1 : a -> b -> c turns left (counter-clockwise)
// -1 : turns right (clockwise)
//  0 : collinear
int orient(Point a, Point b, Point c) { return sign(cross(b - a, c - a)); }

bool is_parallel(Point a, Point b) { return sign(cross(a, b)) == 0; }       // vectors a, b
bool is_perpendicular(Point a, Point b) { return sign(dot(a, b)) == 0; }    // vectors a, b

Point perp(Point a) { return Point(-a.y, a.x); }              // rotate 90 deg counter-clockwise
Point unit(Point a) { return a / len(a); }                    // [D]
Point rotate_ccw(Point a, ld ang) {                           // [D] rotate around origin
    return Point(a.x * cosl(ang) - a.y * sinl(ang), a.x * sinl(ang) + a.y * cosl(ang));
}
Point rotate_around(Point p, Point center, ld ang) {          // [D]
    return center + rotate_ccw(p - center, ang);
}

ld polar_angle(Point a) { return atan2l(a.y, a.x); }                                // in (-pi, pi]
ld angle_between(Point a, Point b) { return atan2l(fabsl(cross(a, b)), dot(a, b)); } // in [0, pi]
ld signed_angle(Point a, Point b) { return atan2l(cross(a, b), dot(a, b)); }         // a -> b ccw, (-pi, pi]
ld angle_at(Point a, Point b, Point c) { return angle_between(a - b, c - b); }      // angle abc in [0, pi]

// ---- sorting by angle (exact for integers, no atan2) ----
// 0 for angle in [0, pi), 1 for angle in [pi, 2pi)
int half(Point a) { return sign(a.y) < 0 || (sign(a.y) == 0 && sign(a.x) < 0); }

// is vector a before vector b when sweeping ccw starting from the +x axis?
bool angle_less(Point a, Point b) {
    if (half(a) != half(b)) return half(a) < half(b);
    return sign(cross(a, b)) > 0;
}
// sort points by angle around center c (ccw from +x axis), ties -> closer first
void sort_by_angle(vector<Point>& v, Point c) {
    sort(v.begin(), v.end(), [&](Point a, Point b) {
        a = a - c, b = b - c;
        if (angle_less(a, b)) return true;
        if (angle_less(b, a)) return false;
        return norm2(a) < norm2(b);
    });
}


// ================================ LINES & SEGMENTS ================================
// A line is given by two distinct points (a, b). Segment = [a, b].

bool on_line(Point a, Point b, Point p) { return orient(a, b, p) == 0; }

// endpoints included; also works when a == b
bool on_segment(Point a, Point b, Point p) {
    return orient(a, b, p) == 0 && sign(dot(a - p, b - p)) <= 0;
}

bool lines_parallel(Point a, Point b, Point c, Point d) { return sign(cross(b - a, d - c)) == 0; }
bool lines_same(Point a, Point b, Point c, Point d) { return lines_parallel(a, b, c, d) && orient(a, b, c) == 0; }

// do segments [a,b] and [c,d] share at least one point? (touching / overlapping counts)
bool segments_intersect(Point a, Point b, Point c, Point d) {
    int o1 = orient(a, b, c), o2 = orient(a, b, d);
    int o3 = orient(c, d, a), o4 = orient(c, d, b);
    if (o1 * o2 < 0 && o3 * o4 < 0) return true;           // proper crossing
    return on_segment(a, b, c) || on_segment(a, b, d) || on_segment(c, d, a) || on_segment(c, d, b);
}

// do segments cross at exactly one interior point of both (no touching, no overlap)?
bool segments_cross_properly(Point a, Point b, Point c, Point d) {
    return orient(a, b, c) * orient(a, b, d) < 0 && orient(c, d, a) * orient(c, d, b) < 0;
}

// does segment [a,b] touch/cross the infinite line (c,d)?
bool segment_line_intersect(Point a, Point b, Point c, Point d) {
    return orient(c, d, a) * orient(c, d, b) <= 0;
}

// [D] intersection of infinite lines (a,b) and (c,d). returns false if parallel (or same line)
bool line_intersection(Point a, Point b, Point c, Point d, Point& res) {
    T den = cross(b - a, d - c);
    if (sign(den) == 0) return false;
    res = a + (b - a) * (cross(c - a, d - c) / den);
    return true;
}

// [D] all common points of segments [a,b] and [c,d]:
// size 0 -> none, size 1 -> the single point, size 2 -> overlap is the segment between them
vector<Point> segment_intersection(Point a, Point b, Point c, Point d) {
    T oa = cross(d - c, a - c), ob = cross(d - c, b - c);
    T oc = cross(b - a, c - a), od = cross(b - a, d - a);
    if (sign(oa) * sign(ob) < 0 && sign(oc) * sign(od) < 0)
        return {(a * ob - b * oa) / (ob - oa)};
    set<Point> s;
    if (on_segment(c, d, a)) s.insert(a);
    if (on_segment(c, d, b)) s.insert(b);
    if (on_segment(a, b, c)) s.insert(c);
    if (on_segment(a, b, d)) s.insert(d);
    return vector<Point>(s.begin(), s.end());
}

// [D] foot of perpendicular from p onto line (a,b)
Point projection(Point a, Point b, Point p) {
    return a + (b - a) * (dot(p - a, b - a) / norm2(b - a));
}
// [D] mirror image of p across line (a,b)
Point reflection(Point a, Point b, Point p) {
    return projection(a, b, p) * 2 - p;
}

ld dist_point_line(Point a, Point b, Point p) {
    return fabsl((ld)cross(b - a, p - a)) / len(b - a);
}
ld dist_point_segment(Point a, Point b, Point p) {
    if (a == b) return dist(a, p);
    if (sign(dot(p - a, b - a)) < 0) return dist(p, a);
    if (sign(dot(p - b, a - b)) < 0) return dist(p, b);
    return dist_point_line(a, b, p);
}
ld dist_segment_segment(Point a, Point b, Point c, Point d) {
    if (segments_intersect(a, b, c, d)) return 0;
    return min({dist_point_segment(a, b, c), dist_point_segment(a, b, d),
                dist_point_segment(c, d, a), dist_point_segment(c, d, b)});
}
// [D] point of segment [a,b] closest to p
Point closest_point_on_segment(Point a, Point b, Point p) {
    if (a == b) return a;
    T t = dot(p - a, b - a);
    if (sign(t) <= 0) return a;
    if (sign(t - norm2(b - a)) >= 0) return b;
    return a + (b - a) * (t / norm2(b - a));
}

// number of integer points on segment [a,b] (endpoints included). integer coords only
ll lattice_points_on_segment(Point a, Point b) {
    return __gcd((ll)abs(a.x - b.x), (ll)abs(a.y - b.y)) + 1;
}

// [D] line a*x + b*y + c = 0  ->  two points on it
pair<Point, Point> line_from_abc(T a, T b, T c) {
    Point p = sign(b) != 0 ? Point(0, -c / b) : Point(-c / a, 0);
    return {p, p + Point(b, -a)};
}
// line through p, q  ->  a*x + b*y + c = 0   (exact for integers)
void line_to_abc(Point p, Point q, T& a, T& b, T& c) {
    a = q.y - p.y, b = p.x - q.x, c = -(a * p.x + b * p.y);
}


// ===================================== POLYGONS =====================================
// Polygon = vector<Point> in order (cw or ccw), no repeated last point.

// signed, TWICE the area. > 0 if counter-clockwise. exact for integers
T polygon_area2(vector<Point>& p) {
    T s = 0;
    for (int i = 0, n = p.size(); i < n; i++) s += cross(p[i], p[(i + 1) % n]);
    return s;
}
ld polygon_area(vector<Point>& p) { return fabsl((ld)polygon_area2(p)) / 2; }

ld polygon_perimeter(vector<Point>& p) {
    ld s = 0;
    for (int i = 0, n = p.size(); i < n; i++) s += dist(p[i], p[(i + 1) % n]);
    return s;
}

void make_ccw(vector<Point>& p) { if (sign(polygon_area2(p)) < 0) reverse(p.begin(), p.end()); }

// all turns go the same way (collinear consecutive points allowed)
bool is_convex(vector<Point>& p) {
    int n = p.size();
    bool pos = false, neg = false;
    for (int i = 0; i < n; i++) {
        int o = orient(p[i], p[(i + 1) % n], p[(i + 2) % n]);
        if (o > 0) pos = true;
        if (o < 0) neg = true;
    }
    return !(pos && neg);
}

// any simple polygon (convex or not, cw or ccw). O(n)
// returns 1 = strictly inside, 0 = on boundary, -1 = strictly outside
int point_in_polygon(vector<Point>& poly, Point p) {
    int n = poly.size();
    bool inside = false;
    for (int i = 0; i < n; i++) {
        Point a = poly[i], b = poly[(i + 1) % n];
        if (on_segment(a, b, p)) return 0;
        if (sign(a.y - b.y) > 0) swap(a, b);                  // a = lower endpoint
        // edge crosses the horizontal ray going right from p (half-open rule on y)
        if (sign(a.y - p.y) <= 0 && sign(b.y - p.y) > 0 && orient(a, b, p) > 0) inside = !inside;
    }
    return inside ? 1 : -1;
}

// convex polygon in CCW order WITHOUT collinear vertices (e.g. output of convex_hull). O(log n)
// returns 1 = strictly inside, 0 = on boundary, -1 = strictly outside
int point_in_convex_polygon(vector<Point>& poly, Point p) {
    int n = poly.size();
    if (n == 1) return poly[0] == p ? 0 : -1;
    if (n == 2) return on_segment(poly[0], poly[1], p) ? 0 : -1;
    Point o = poly[0];
    if (orient(o, poly[1], p) < 0 || orient(o, poly[n - 1], p) > 0) return -1;
    int lo = 1, hi = n - 1;                                     // find wedge (o, poly[lo], poly[lo+1])
    while (hi - lo > 1) {
        int mid = (lo + hi) / 2;
        if (orient(o, poly[mid], p) >= 0) lo = mid; else hi = mid;
    }
    int s = orient(poly[lo], poly[lo + 1], p);
    if (s < 0) return -1;
    if (s == 0) return 0;
    if (lo == 1 && orient(o, poly[1], p) == 0) return 0;
    if (lo + 1 == n - 1 && orient(o, poly[n - 1], p) == 0) return 0;
    return 1;
}

// [D] center of mass of the polygon's area
Point polygon_centroid(vector<Point>& p) {
    Point c(0, 0);
    T a2 = 0;
    for (int i = 0, n = p.size(); i < n; i++) {
        T cr = cross(p[i], p[(i + 1) % n]);
        c = c + (p[i] + p[(i + 1) % n]) * cr;
        a2 += cr;
    }
    return c / (a2 * 3);
}

// Pick's theorem: area = I + B/2 - 1. integer coords only
ll boundary_lattice_points(vector<Point>& p) {
    ll b = 0;
    for (int i = 0, n = p.size(); i < n; i++)
        b += __gcd((ll)abs(p[i].x - p[(i + 1) % n].x), (ll)abs(p[i].y - p[(i + 1) % n].y));
    return b;
}
ll interior_lattice_points(vector<Point>& p) {
    return ((ll)abs(polygon_area2(p)) - boundary_lattice_points(p) + 2) / 2;
}

// Andrew's monotone chain. O(n log n). returns hull in CCW order, starting from the lowest-left point.
// keep_collinear = false -> only corner points; true -> also points lying on hull edges
vector<Point> convex_hull(vector<Point> pts, bool keep_collinear = false) {
    sort(pts.begin(), pts.end());
    pts.erase(unique(pts.begin(), pts.end()), pts.end());
    int n = pts.size();
    if (n <= 2) return pts;
    bool all_collinear = true;
    for (int i = 2; i < n; i++) if (orient(pts[0], pts[1], pts[i]) != 0) all_collinear = false;
    if (all_collinear) return keep_collinear ? pts : vector<Point>{pts[0], pts[n - 1]};

    auto bad = [&](Point a, Point b, Point c) {
        int o = orient(a, b, c);
        return keep_collinear ? o < 0 : o <= 0;
    };
    vector<Point> h(2 * n);
    int k = 0;
    for (int i = 0; i < n; i++) {                                  // lower hull
        while (k >= 2 && bad(h[k - 2], h[k - 1], pts[i])) k--;
        h[k++] = pts[i];
    }
    for (int i = n - 2, t = k + 1; i >= 0; i--) {                  // upper hull
        while (k >= t && bad(h[k - 2], h[k - 1], pts[i])) k--;
        h[k++] = pts[i];
    }
    h.resize(k - 1);
    return h;
}

// rotating calipers. hull = CCW, no collinear points. returns SQUARED max distance between two points
T convex_diameter2(vector<Point>& h) {
    int n = h.size();
    if (n == 1) return 0;
    if (n == 2) return dist2(h[0], h[1]);
    T best = 0;
    for (int i = 0, j = 1; i < n; i++) {
        int ni = (i + 1) % n;
        while (sign(cross(h[ni] - h[i], h[(j + 1) % n] - h[j])) > 0) j = (j + 1) % n;
        best = max({best, dist2(h[i], h[j]), dist2(h[ni], h[j])});
    }
    return best;
}

// min distance between two parallel lines enclosing the polygon. hull = CCW, no collinear points
ld convex_width(vector<Point>& h) {
    int n = h.size();
    if (n <= 2) return 0;
    ld best = 1e30;
    for (int i = 0, j = 1; i < n; i++) {
        Point a = h[i], b = h[(i + 1) % n];
        while (sign(cross(a, b, h[(j + 1) % n]) - cross(a, b, h[j])) > 0) j = (j + 1) % n;
        best = min(best, fabsl((ld)cross(a, b, h[j])) / len(b - a));
    }
    return best;
}

// Minkowski sum of two convex polygons (both CCW). O(n + m). exact for integers
vector<Point> minkowski_sum(vector<Point> A, vector<Point> B) {
    auto start_from_bottom = [](vector<Point>& v) {
        int pos = 0;
        for (int i = 1; i < (int)v.size(); i++)
            if (sign(v[i].y - v[pos].y) < 0 || (sign(v[i].y - v[pos].y) == 0 && sign(v[i].x - v[pos].x) < 0)) pos = i;
        rotate(v.begin(), v.begin() + pos, v.end());
    };
    start_from_bottom(A), start_from_bottom(B);
    A.push_back(A[0]), A.push_back(A[1]);
    B.push_back(B[0]), B.push_back(B[1]);
    vector<Point> res;
    size_t i = 0, j = 0;
    while (i < A.size() - 2 || j < B.size() - 2) {
        res.push_back(A[i] + B[j]);
        int c = sign(cross(A[i + 1] - A[i], B[j + 1] - B[j]));
        if (c >= 0 && i < A.size() - 2) i++;
        if (c <= 0 && j < B.size() - 2) j++;
    }
    return res;
}

// [D] cut a CONVEX polygon by line a->b, keep the part on the LEFT side (orient >= 0)
vector<Point> cut_polygon(vector<Point>& poly, Point a, Point b) {
    vector<Point> res;
    for (int i = 0, n = poly.size(); i < n; i++) {
        Point cur = poly[i], nxt = poly[(i + 1) % n];
        T c1 = cross(b - a, cur - a), c2 = cross(b - a, nxt - a);
        if (sign(c1) >= 0) res.push_back(cur);
        if (sign(c1) * sign(c2) < 0) res.push_back((cur * c2 - nxt * c1) / (c2 - c1));
    }
    return res;
}

// SQUARED distance of the closest pair of points. divide & conquer O(n log n). n >= 2
T closest_pair_dist2(vector<Point> a) {
    sort(a.begin(), a.end());
    T best = dist2(a[0], a[1]);
    vector<Point> tmp(a.size());
    auto by_y = [](Point p, Point q) { return sign(p.y - q.y) < 0; };
    function<void(int, int)> solve = [&](int l, int r) {          // [l, r)
        if (r - l <= 3) {
            for (int i = l; i < r; i++)
                for (int j = i + 1; j < r; j++) best = min(best, dist2(a[i], a[j]));
            sort(a.begin() + l, a.begin() + r, by_y);
            return;
        }
        int m = (l + r) / 2;
        T midx = a[m].x;
        solve(l, m), solve(m, r);
        merge(a.begin() + l, a.begin() + m, a.begin() + m, a.begin() + r, tmp.begin(), by_y);
        copy(tmp.begin(), tmp.begin() + (r - l), a.begin() + l);
        int k = 0;
        for (int i = l; i < r; i++) {
            T dx = a[i].x - midx;
            if (sign(dx * dx - best) >= 0) continue;
            for (int j = k - 1; j >= 0; j--) {
                T dy = a[i].y - tmp[j].y;
                if (sign(dy * dy - best) >= 0) break;
                best = min(best, dist2(a[i], tmp[j]));
            }
            tmp[k++] = a[i];
        }
    };
    solve(0, a.size());
    return best;
}

// [D] intersection of half-planes. each half-plane = (a, b): keeps the LEFT side of a->b.
// returns the CCW polygon of the intersection (empty if empty/degenerate). O(n log n)
// a big bounding box is added so the result is always bounded.
vector<Point> half_plane_intersection(vector<pair<Point, Point>> lines) {
    struct HalfPlane {
        Point p, d; ld ang;
        bool out(Point r) { return sign(cross(d, r - p)) < 0; }
    };
    auto inter = [](HalfPlane& s, HalfPlane& t) {
        return s.p + s.d * (cross(t.p - s.p, t.d) / cross(s.d, t.d));
    };
    const T INF = 1e9;
    Point box[4] = {Point(INF, INF), Point(-INF, INF), Point(-INF, -INF), Point(INF, -INF)};
    for (int i = 0; i < 4; i++) lines.push_back({box[i], box[(i + 1) % 4]});
    vector<HalfPlane> H;
    for (auto& l : lines) {
        Point d = l.second - l.first;
        H.push_back({l.first, d, atan2l(d.y, d.x)});
    }
    sort(H.begin(), H.end(), [](HalfPlane& x, HalfPlane& y) { return x.ang < y.ang; });

    deque<HalfPlane> dq;
    for (auto& h : H) {
        while (dq.size() > 1 && h.out(inter(dq[dq.size() - 1], dq[dq.size() - 2]))) dq.pop_back();
        while (dq.size() > 1 && h.out(inter(dq[0], dq[1]))) dq.pop_front();
        if (!dq.empty() && sign(cross(h.d, dq.back().d)) == 0) {
            if (sign(dot(h.d, dq.back().d)) < 0) return {};        // opposite parallel -> empty
            if (h.out(dq.back().p)) dq.pop_back();                 // same direction: keep tighter
            else continue;
        }
        dq.push_back(h);
    }
    while (dq.size() > 2 && dq[0].out(inter(dq[dq.size() - 1], dq[dq.size() - 2]))) dq.pop_back();
    while (dq.size() > 2 && dq.back().out(inter(dq[0], dq[1]))) dq.pop_front();
    if (dq.size() < 3) return {};
    vector<Point> res;
    for (int i = 0, n = dq.size(); i < n; i++) res.push_back(inter(dq[i], dq[(i + 1) % n]));
    return res;
}


// ====================================== CIRCLES ======================================
// A circle is (center c, radius r).

// 1 = strictly inside, 0 = on circle, -1 = strictly outside. exact for integers
int point_in_circle(Point c, T r, Point p) { return sign(r * r - dist2(c, p)); }

// number of common tangents (exact for integers):
// 4 = separate, 3 = touch outside, 2 = intersect at 2 points, 1 = touch inside,
// 0 = one strictly inside the other, -1 = same circle
int circle_relation(Point c1, T r1, Point c2, T r2) {
    T d2 = dist2(c1, c2), sum = (r1 + r2) * (r1 + r2), dif = (r1 - r2) * (r1 - r2);
    if (sign(d2) == 0 && sign(r1 - r2) == 0) return -1;
    if (sign(d2 - sum) > 0) return 4;
    if (sign(d2 - sum) == 0) return 3;
    if (sign(d2 - dif) > 0) return 2;
    if (sign(d2 - dif) == 0) return 1;
    return 0;
}

// [D] intersection points of circle with infinite line (a,b). 0, 1 (tangent) or 2 points
vector<Point> circle_line_intersection(Point c, T r, Point a, Point b) {
    Point d = b - a;
    Point p = a + d * (dot(c - a, d) / norm2(d));             // projection of center
    T h2 = r * r - dist2(c, p);
    if (sign(h2) < 0) return {};
    if (sign(h2) == 0) return {p};
    Point off = d * (sqrtl(h2) / len(d));
    return {p - off, p + off};
}

// [D] intersection points of two circles. 0, 1 (tangent) or 2 points. same circle -> {}
vector<Point> circle_circle_intersection(Point c1, T r1, Point c2, T r2) {
    Point d = c2 - c1;
    T d2 = norm2(d);
    if (sign(d2) == 0) return {};
    T sum = (r1 + r2) * (r1 + r2), dif = (r1 - r2) * (r1 - r2);
    if (sign(d2 - sum) > 0 || sign(d2 - dif) < 0) return {};
    T a = (d2 + r1 * r1 - r2 * r2) / (2 * d2);                // mid = c1 + d * a
    Point mid = c1 + d * a;
    if (sign(d2 - sum) == 0 || sign(d2 - dif) == 0) return {mid};
    T h = sqrtl(max((T)0, r1 * r1 / d2 - a * a));
    return {mid + perp(d) * h, mid - perp(d) * h};
}

// [D] points on the circle where tangents from p touch it. 0 (p inside), 1 (p on circle) or 2
vector<Point> tangent_points_from_point(Point c, T r, Point p) {
    Point d = p - c;
    T d2 = norm2(d);
    int s = sign(d2 - r * r);
    if (s < 0) return {};
    if (s == 0) return {p};
    Point base = c + d * (r * r / d2);
    Point off = perp(d) * (r * sqrtl(d2 - r * r) / d2);
    return {base + off, base - off};
}

// [D] common tangents of two circles. each = (touch point on circle 1, touch point on circle 2)
// outer tangents first, then inner. if touch point is same on both (tangent circles), pair has equal points.
vector<pair<Point, Point>> common_tangents(Point c1, T r1, Point c2, T r2) {
    vector<pair<Point, Point>> res;
    Point d = c2 - c1;
    T d2 = norm2(d);
    if (sign(d2) == 0) return res;
    for (int inner = 0; inner < 2; inner++) {
        T R2 = inner ? -r2 : r2;
        T dr = r1 - R2, h2 = d2 - dr * dr;
        if (sign(h2) < 0) continue;
        h2 = max((T)0, h2);
        for (int s : {1, -1}) {
            Point v = (d * dr + perp(d) * (sqrtl(h2) * s)) / d2;
            res.push_back({c1 + v * r1, c2 + v * R2});
            if (sign(h2) == 0) break;
        }
    }
    return res;
}

// [D] circle through 3 non-collinear points
Point circumcenter(Point a, Point b, Point c) {
    b = b - a, c = c - a;
    return a + Point(c.y * norm2(b) - b.y * norm2(c), b.x * norm2(c) - c.x * norm2(b)) / (cross(b, c) * 2);
}
ld circumradius(Point a, Point b, Point c) {
    return dist(a, b) * dist(b, c) * dist(c, a) / fabsl((ld)cross(a, b, c)) / 2;
}
// [D] center of inscribed circle
Point incenter(Point a, Point b, Point c) {
    T la = dist(b, c), lb = dist(c, a), lc = dist(a, b);
    return (a * la + b * lb + c * lc) / (la + lb + lc);
}
ld inradius(Point a, Point b, Point c) {
    return fabsl((ld)cross(a, b, c)) / (dist(a, b) + dist(b, c) + dist(c, a));
}

// [D] smallest circle containing all points. expected O(n) (Welzl, randomized)
pair<Point, ld> min_enclosing_circle(vector<Point> p) {
    shuffle(p.begin(), p.end(), mt19937(1234567));
    Point o = p[0];
    ld r = 0;
    for (int i = 0; i < (int)p.size(); i++) if (sign(dist(o, p[i]) - r) > 0) {
        o = p[i], r = 0;
        for (int j = 0; j < i; j++) if (sign(dist(o, p[j]) - r) > 0) {
            o = (p[i] + p[j]) / 2, r = dist(o, p[i]);
            for (int k = 0; k < j; k++) if (sign(dist(o, p[k]) - r) > 0) {
                o = circumcenter(p[i], p[j], p[k]), r = dist(o, p[i]);
            }
        }
    }
    return {o, r};
}

// area of intersection of two circles (works with integer input)
ld circle_circle_intersection_area(Point c1, T r1, Point c2, T r2) {
    T d2 = dist2(c1, c2);
    if (sign(d2 - (r1 + r2) * (r1 + r2)) >= 0) return 0;                       // apart / touching
    if (sign(d2 - (r1 - r2) * (r1 - r2)) <= 0) return PI * min(r1, r2) * min(r1, r2); // one inside
    ld d = sqrtl((ld)d2), R1 = r1, R2 = r2;
    ld a1 = acosl(max((ld)-1, min((ld)1, (d * d + R1 * R1 - R2 * R2) / (2 * d * R1))));
    ld a2 = acosl(max((ld)-1, min((ld)1, (d * d + R2 * R2 - R1 * R1) / (2 * d * R2))));
    return R1 * R1 * (a1 - sinl(2 * a1) / 2) + R2 * R2 * (a2 - sinl(2 * a2) / 2);
}

// area of intersection of a circle and any simple polygon (works with integer input)
ld circle_polygon_intersection_area(Point c, T r, vector<Point>& poly) {
    ld R = r;
    auto arg = [](ld ax, ld ay, ld bx, ld by) { return atan2l(ax * by - ay * bx, ax * bx + ay * by); };
    // signed area of (circle at origin) ∩ (triangle origin, p, q)
    auto tri = [&](ld px, ld py, ld qx, ld qy) -> ld {
        ld dx = qx - px, dy = qy - py, dd = dx * dx + dy * dy;
        if (dd == 0) return 0;
        ld a = (dx * px + dy * py) / dd, b = (px * px + py * py - R * R) / dd, det = a * a - b;
        if (det <= 0) return arg(px, py, qx, qy) * R * R / 2;
        ld s = max((ld)0, -a - sqrtl(det)), t = min((ld)1, -a + sqrtl(det));
        if (t < 0 || 1 <= s) return arg(px, py, qx, qy) * R * R / 2;
        ld ux = px + dx * s, uy = py + dy * s, vx = px + dx * t, vy = py + dy * t;
        return arg(px, py, ux, uy) * R * R / 2 + (ux * vy - uy * vx) / 2 + arg(vx, vy, qx, qy) * R * R / 2;
    };
    ld sum = 0;
    for (int i = 0, n = poly.size(); i < n; i++) {
        Point p = poly[i] - c, q = poly[(i + 1) % n] - c;
        sum += tri(p.x, p.y, q.x, q.y);
    }
    return fabsl(sum);
}


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // example: convex hull + area + point queries
    int n;
    cin >> n;
    vector<Point> pts(n);
    for (auto& p : pts) cin >> p;
    vector<Point> hull = convex_hull(pts);
    cout << "hull size: " << hull.size() << "\n";
    cout << "twice area: " << polygon_area2(hull) << "\n";
    int q;
    cin >> q;
    while (q--) {
        Point p;
        cin >> p;
        cout << point_in_convex_polygon(hull, p) << "\n";    // 1 in, 0 boundary, -1 out
    }
}
