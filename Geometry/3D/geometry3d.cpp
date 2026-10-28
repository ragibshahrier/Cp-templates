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
//   Functions returning ld (distances, angles, areas, volumes) work fine with T = long long.
// * ll overflow: triple product / orient3 is CUBIC -> with ll keep coords up to ~1e6.
//   cross / dot are quadratic -> coords up to ~1e9 are ok for those alone.
// ============================================================================================

const ld PI = acosl(-1);

// -1 if x < 0, 0 if x == 0, +1 if x > 0 (with EPS)
int sign(ld x) { return (x > EPS) - (x < -EPS); }

struct Point3 {
    T x, y, z;
    Point3(T x = 0, T y = 0, T z = 0) : x(x), y(y), z(z) {}
    Point3 operator+(Point3 p) const { return Point3(x + p.x, y + p.y, z + p.z); }
    Point3 operator-(Point3 p) const { return Point3(x - p.x, y - p.y, z - p.z); }
    Point3 operator*(T k) const { return Point3(x * k, y * k, z * k); }
    Point3 operator/(T k) const { return Point3(x / k, y / k, z / k); }
    bool operator==(Point3 p) const { return sign(x - p.x) == 0 && sign(y - p.y) == 0 && sign(z - p.z) == 0; }
    bool operator!=(Point3 p) const { return !(*this == p); }
    bool operator<(Point3 p) const {
        if (sign(x - p.x) != 0) return x < p.x;
        if (sign(y - p.y) != 0) return y < p.y;
        return sign(z - p.z) < 0;
    }
};
istream& operator>>(istream& in, Point3& p) { return in >> p.x >> p.y >> p.z; }
ostream& operator<<(ostream& out, Point3 p) { return out << p.x << " " << p.y << " " << p.z; }


// ======================================= BASICS =======================================

T dot(Point3 a, Point3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
Point3 cross(Point3 a, Point3 b) {
    return Point3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}
T norm2(Point3 a) { return dot(a, a); }                                    // |a|^2
T dist2(Point3 a, Point3 b) { return norm2(a - b); }
ld len(Point3 a) { return sqrtl((ld)a.x * a.x + (ld)a.y * a.y + (ld)a.z * a.z); }   // overflow safe
ld dist(Point3 a, Point3 b) { return len(a - b); }
Point3 unit(Point3 a) { return a / len(a); }                               // [D]

bool is_zero(Point3 a) { return sign(a.x) == 0 && sign(a.y) == 0 && sign(a.z) == 0; }
bool is_parallel(Point3 a, Point3 b) { return is_zero(cross(a, b)); }       // vectors
bool is_perpendicular(Point3 a, Point3 b) { return sign(dot(a, b)) == 0; }  // vectors

// a . (b x c) = 6 * signed volume of tetrahedron (0, a, b, c)
T triple(Point3 a, Point3 b, Point3 c) { return dot(a, cross(b, c)); }

// +1 : d is on the side where (b-a) x (c-a) points (abc looks counter-clockwise from d)
// -1 : other side,  0 : a, b, c, d coplanar
int orient3(Point3 a, Point3 b, Point3 c, Point3 d) { return sign(triple(b - a, c - a, d - a)); }

bool collinear(Point3 a, Point3 b, Point3 c) { return is_zero(cross(b - a, c - a)); }
bool coplanar(Point3 a, Point3 b, Point3 c, Point3 d) { return orient3(a, b, c, d) == 0; }

ld angle_between(Point3 a, Point3 b) { return atan2l(len(cross(a, b)), dot(a, b)); }   // [0, pi]

// [D] rotate p around the axis through the origin with direction `axis`, by `ang` (right-hand rule)
Point3 rotate_around_axis(Point3 p, Point3 axis, ld ang) {
    Point3 k = unit(axis);
    return p * cosl(ang) + cross(k, p) * sinl(ang) + k * (dot(k, p) * (1 - cosl(ang)));
}


// ================================ LINES & SEGMENTS ================================
// A line is given by two distinct points (a, b). Segment = [a, b].

bool on_line(Point3 a, Point3 b, Point3 p) { return collinear(a, b, p); }

bool on_segment(Point3 a, Point3 b, Point3 p) {
    return collinear(a, b, p) && sign(dot(a - p, b - p)) <= 0;
}

ld dist_point_line(Point3 a, Point3 b, Point3 p) { return len(cross(b - a, p - a)) / len(b - a); }

ld dist_point_segment(Point3 a, Point3 b, Point3 p) {
    if (a == b) return dist(a, p);
    if (sign(dot(p - a, b - a)) < 0) return dist(p, a);
    if (sign(dot(p - b, a - b)) < 0) return dist(p, b);
    return dist_point_line(a, b, p);
}

// [D] foot of perpendicular from p onto line (a,b)
Point3 projection_on_line(Point3 a, Point3 b, Point3 p) {
    return a + (b - a) * (dot(p - a, b - a) / norm2(b - a));
}

// shortest distance between infinite lines (a,b) and (c,d) (skew, parallel or intersecting)
ld dist_line_line(Point3 a, Point3 b, Point3 c, Point3 d) {
    Point3 n = cross(b - a, d - c);
    if (is_zero(n)) return dist_point_line(a, b, c);              // parallel
    return fabsl((ld)dot(c - a, n)) / len(n);
}

// [D] closest points between NON-parallel lines (a,b) and (c,d): p1 on first, p2 on second
// returns false if parallel. lines intersect iff p1 == p2
bool closest_points_lines(Point3 a, Point3 b, Point3 c, Point3 d, Point3& p1, Point3& p2) {
    Point3 d1 = b - a, d2 = d - c, n = cross(d1, d2);
    if (is_zero(n)) return false;
    p1 = a + d1 * (dot(cross(c - a, d2), n) / norm2(n));
    p2 = c + d2 * (dot(cross(c - a, d1), n) / norm2(n));
    return true;
}

// [D] intersection of infinite lines. false if parallel or skew
bool line_line_intersection(Point3 a, Point3 b, Point3 c, Point3 d, Point3& res) {
    if (!coplanar(a, b, c, d) || is_parallel(b - a, d - c)) return false;
    Point3 n = cross(b - a, d - c);
    res = a + (b - a) * (dot(cross(c - a, d - c), n) / norm2(n));
    return true;
}

// do segments [a,b] and [c,d] share a point? (touching / overlapping counts). exact for integers
bool segments_intersect(Point3 a, Point3 b, Point3 c, Point3 d) {
    if (!coplanar(a, b, c, d)) return false;
    if (on_segment(a, b, c) || on_segment(a, b, d) || on_segment(c, d, a) || on_segment(c, d, b)) return true;
    // coplanar: proper crossing <=> c, d on opposite sides of ab AND a, b on opposite sides of cd
    Point3 n1 = cross(b - a, c - a), n2 = cross(b - a, d - a);
    Point3 n3 = cross(d - c, a - c), n4 = cross(d - c, b - c);
    return sign((ld)n1.x * n2.x + (ld)n1.y * n2.y + (ld)n1.z * n2.z) < 0 &&
           sign((ld)n3.x * n4.x + (ld)n3.y * n4.y + (ld)n3.z * n4.z) < 0;
}

// shortest distance between segments [p1,q1] and [p2,q2] (works with integer input)
ld dist_segment_segment(Point3 p1, Point3 q1, Point3 p2, Point3 q2) {
    Point3 d1 = q1 - p1, d2 = q2 - p2, r = p1 - p2;
    ld a = norm2(d1), e = norm2(d2), f = dot(d2, r);
    auto clamp01 = [](ld v) { return max((ld)0, min((ld)1, v)); };
    ld s, t;
    if (sign(a) == 0 && sign(e) == 0) return dist(p1, p2);
    if (sign(a) == 0) s = 0, t = clamp01(f / e);
    else {
        ld c = dot(d1, r);
        if (sign(e) == 0) t = 0, s = clamp01(-c / a);
        else {
            ld b = dot(d1, d2), den = a * e - b * b;
            s = sign(den) > 0 ? clamp01((b * f - c * e) / den) : 0;
            t = (b * s + f) / e;
            if (t < 0) t = 0, s = clamp01(-c / a);
            else if (t > 1) t = 1, s = clamp01((b - c) / a);
        }
    }
    ld x = p1.x + d1.x * s - p2.x - d2.x * t;
    ld y = p1.y + d1.y * s - p2.y - d2.y * t;
    ld z = p1.z + d1.z * s - p2.z - d2.z * t;
    return sqrtl(x * x + y * y + z * z);
}


// ====================================== PLANES ======================================
// Plane: dot(n, X) = d   (n = normal, not necessarily unit)

struct Plane {
    Point3 n;
    T d;
};
Plane plane_from_points(Point3 a, Point3 b, Point3 c) {       // a, b, c not collinear
    Point3 n = cross(b - a, c - a);
    return {n, dot(n, a)};
}
Plane plane_from_normal_point(Point3 n, Point3 p) { return {n, dot(n, p)}; }

// +1 : p on the side n points to, -1 : other side, 0 : on plane
int side_of_plane(Plane pl, Point3 p) { return sign(dot(pl.n, p) - pl.d); }

ld signed_dist_point_plane(Plane pl, Point3 p) { return (ld)(dot(pl.n, p) - pl.d) / len(pl.n); }
ld dist_point_plane(Plane pl, Point3 p) { return fabsl(signed_dist_point_plane(pl, p)); }

// [D]
Point3 project_onto_plane(Plane pl, Point3 p) { return p - pl.n * ((dot(pl.n, p) - pl.d) / norm2(pl.n)); }
Point3 reflect_across_plane(Plane pl, Point3 p) { return p - pl.n * (2 * (dot(pl.n, p) - pl.d) / norm2(pl.n)); }

// [D] intersection of infinite line (a,b) with plane. false if parallel (or lying inside)
bool line_plane_intersection(Point3 a, Point3 b, Plane pl, Point3& res) {
    T den = dot(pl.n, b - a);
    if (sign(den) == 0) return false;
    res = a + (b - a) * ((pl.d - dot(pl.n, a)) / den);
    return true;
}

// [D] intersection line of two planes: point + t * dir. false if parallel
bool plane_plane_intersection(Plane p1, Plane p2, Point3& point, Point3& dir) {
    dir = cross(p1.n, p2.n);
    if (is_zero(dir)) return false;
    point = cross(p2.n * p1.d - p1.n * p2.d, dir) / norm2(dir);
    return true;
}

ld angle_between_planes(Plane p1, Plane p2) {                  // in [0, pi/2]
    ld a = angle_between(p1.n, p2.n);
    return min(a, PI - a);
}


// ================================ TRIANGLES & SOLIDS ================================

ld triangle_area(Point3 a, Point3 b, Point3 c) { return len(cross(b - a, c - a)) / 2; }

// p must be on the plane of abc (otherwise returns -1).
// returns 1 = strictly inside, 0 = on boundary, -1 = outside
int point_in_triangle(Point3 a, Point3 b, Point3 c, Point3 p) {
    if (!coplanar(a, b, c, p)) return -1;
    Point3 n = cross(b - a, c - a);
    auto side = [&](Point3 u, Point3 v) {                       // sign of (v-u)x(p-u) along n
        Point3 w = cross(v - u, p - u);
        return sign((ld)w.x * n.x + (ld)w.y * n.y + (ld)w.z * n.z);
    };
    int s1 = side(a, b), s2 = side(b, c), s3 = side(c, a);
    if (s1 < 0 || s2 < 0 || s3 < 0) return -1;
    if (s1 == 0 || s2 == 0 || s3 == 0) return 0;
    return 1;
}

// does segment [p,q] hit triangle abc? (touching counts; segment lying IN the plane -> false)
// exact for integers
bool segment_intersects_triangle(Point3 p, Point3 q, Point3 a, Point3 b, Point3 c) {
    int sp = orient3(a, b, c, p), sq = orient3(a, b, c, q);
    if (sp * sq > 0 || (sp == 0 && sq == 0)) return false;
    int s1 = orient3(p, q, a, b), s2 = orient3(p, q, b, c), s3 = orient3(p, q, c, a);
    return !((s1 < 0 || s2 < 0 || s3 < 0) && (s1 > 0 || s2 > 0 || s3 > 0));
}

// 6 * signed volume of tetrahedron abcd. exact for integers
T tetra_volume6(Point3 a, Point3 b, Point3 c, Point3 d) { return triple(b - a, c - a, d - a); }

// Face of a polyhedron = indices into the point array, CCW when seen from outside
struct Face { int a, b, c; };

ld polyhedron_volume(vector<Point3>& p, vector<Face>& faces) {
    ld v = 0;
    for (auto& f : faces) v += triple(p[f.a], p[f.b], p[f.c]);
    return fabsl(v) / 6;
}
ld polyhedron_surface_area(vector<Point3>& p, vector<Face>& faces) {
    ld s = 0;
    for (auto& f : faces) s += triangle_area(p[f.a], p[f.b], p[f.c]);
    return s;
}

// 3D convex hull, incremental O(n^2) time and memory (n up to ~3000).
// returns triangular faces (indices into p), each CCW seen from outside -> normal points out.
// coplanar faces come triangulated; points lying inside faces/edges are not used.
// returns {} if all points are coplanar.
vector<Face> convex_hull_3d(vector<Point3>& p) {
    int n = p.size();
    // find 4 non-coplanar points to start
    int i1 = -1, i2 = -1, i3 = -1;
    for (int i = 1; i < n && i1 < 0; i++) if (p[i] != p[0]) i1 = i;
    if (i1 < 0) return {};
    for (int i = 1; i < n && i2 < 0; i++) if (!collinear(p[0], p[i1], p[i])) i2 = i;
    if (i2 < 0) return {};
    for (int i = 1; i < n && i3 < 0; i++) if (!coplanar(p[0], p[i1], p[i2], p[i])) i3 = i;
    if (i3 < 0) return {};

    vector<Face> F;
    vector<bool> alive;
    vector<vector<int>> owner(n, vector<int>(n, -1));    // owner[u][v] = face having directed edge u->v
    auto add_face = [&](int a, int b, int c) {
        owner[a][b] = owner[b][c] = owner[c][a] = F.size();
        F.push_back({a, b, c});
        alive.push_back(true);
    };
    auto sees = [&](Face& f, int i) { return orient3(p[f.a], p[f.b], p[f.c], p[i]) > 0; };

    int s[4] = {0, i1, i2, i3};
    for (int k = 0; k < 4; k++) {                         // initial tetrahedron, faces outward
        int a = s[k], b = s[(k + 1) % 4], c = s[(k + 2) % 4], other = s[(k + 3) % 4];
        if (orient3(p[a], p[b], p[c], p[other]) > 0) swap(b, c);
        add_face(a, b, c);
    }
    for (int i = 0; i < n; i++) {
        if (i == 0 || i == i1 || i == i2 || i == i3) continue;
        vector<int> dead;
        for (int j = 0; j < (int)F.size(); j++)
            if (alive[j] && sees(F[j], i)) alive[j] = false, dead.push_back(j);
        for (int j : dead) {
            int e[3] = {F[j].a, F[j].b, F[j].c};
            for (int k = 0; k < 3; k++) {
                int u = e[k], v = e[(k + 1) % 3];
                if (alive[owner[v][u]]) add_face(u, v, i);    // horizon edge
            }
        }
    }
    vector<Face> res;
    for (int j = 0; j < (int)F.size(); j++) if (alive[j]) res.push_back(F[j]);
    return res;
}


// ====================================== SPHERES ======================================

// 1 = strictly inside, 0 = on sphere, -1 = outside. exact for integers
int point_in_sphere(Point3 c, T r, Point3 p) { return sign(r * r - dist2(c, p)); }

// [D] intersection of sphere with infinite line (a,b). 0, 1 or 2 points
vector<Point3> sphere_line_intersection(Point3 c, T r, Point3 a, Point3 b) {
    Point3 d = b - a;
    Point3 p = a + d * (dot(c - a, d) / norm2(d));
    T h2 = r * r - dist2(c, p);
    if (sign(h2) < 0) return {};
    if (sign(h2) == 0) return {p};
    Point3 off = d * (sqrtl(h2) / len(d));
    return {p - off, p + off};
}

// [D] point on sphere of radius r from latitude/longitude in DEGREES
Point3 from_lat_lon(T r, ld lat, ld lon) {
    lat *= PI / 180, lon *= PI / 180;
    return Point3(r * cosl(lat) * cosl(lon), r * cosl(lat) * sinl(lon), r * sinl(lat));
}

// shortest distance along the surface of a sphere of radius r (lat/lon in DEGREES)
ld great_circle_distance(ld r, ld lat1, ld lon1, ld lat2, ld lon2) {
    lat1 *= PI / 180, lon1 *= PI / 180, lat2 *= PI / 180, lon2 *= PI / 180;
    ld h = sinl((lat2 - lat1) / 2) * sinl((lat2 - lat1) / 2) +
           cosl(lat1) * cosl(lat2) * sinl((lon2 - lon1) / 2) * sinl((lon2 - lon1) / 2);
    return 2 * r * asinl(min((ld)1, sqrtl(h)));
}


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // example: volume and surface area of the convex hull
    int n;
    cin >> n;
    vector<Point3> p(n);
    for (auto& x : p) cin >> x;
    vector<Face> hull = convex_hull_3d(p);
    cout << fixed << setprecision(6);
    cout << "faces: " << hull.size() << "\n";
    cout << "volume: " << polyhedron_volume(p, hull) << "\n";
    cout << "surface: " << polyhedron_surface_area(p, hull) << "\n";
}
