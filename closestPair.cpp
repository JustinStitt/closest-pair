#include <bits/stdc++.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

#define DEFAULT_POINTS 10000
#define DEFAULT_LOWER_BOUND -100000
#define DEFAULT_UPPER_BOUND  100000


struct xy{
    double x, y;
    xy(double _x, double _y) : x(_x), y(_y) {}
};

struct pointPair{
    xy* p1, *p2;
    double dist;
    pointPair() : p1(nullptr), p2(nullptr), 
                dist(numeric_limits<double>::max()) {}
    pointPair(xy& _p1, xy& _p2) : p1(&_p1), p2(&_p2), 
                dist(numeric_limits<double>::max()) {}
};

ostream& operator<<(ostream& os, const xy& p){
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

ostream& operator<<(ostream& os, const pointPair& p){
    os << *p.p1 << " " << *p.p2 << " distance: " << setprecision(6) << p.dist;
    return os;
}

double distance(const xy& p1, const xy& p2, bool fast = false){
    double dist =  ((p2.x-p1.x) * (p2.x-p1.x) + 
                    (p2.y - p1.y) * (p2.y - p1.y));
    if(!fast)
        dist = sqrt(dist);
    return dist;
}

void sortByX(vector<xy>& points){
    sort(points.begin(), points.end(), [&](auto l, auto r){
        return l.x < r.x;
    });
}

void sortByY(vector<xy>& points){
    sort(points.begin(), points.end(), [&](auto l, auto r){
        return l.y < r.y;
    });
}

vector<xy> createStrip(const vector<xy>& points, int l, int r, double med_x, double d){
    vector<xy> strip;

    for(int x = l; x <= r; ++x){
        int xdist = abs(points[x].x - med_x);
        if(xdist <= d)
            strip.push_back(xy(points[x]));
        else if(xdist > d) break;
    }
    sortByY(strip);
    return strip;
}
/*
 (1,6), (2,4), (3, 1), (4, 2) . . .
*/
pointPair closestPairBruteForce(const vector<xy>& points, int l, int r, bool fast = false){
    pointPair best;
    for(int x = l; x < r; ++x){
        for(int y = x + 1; y <= r; ++y){
            if(double d = distance(points[x], points[y], fast); d < best.dist){
                best.dist = d;
                best.p1  = (xy*)&points[x];
                best.p2  = (xy*)&points[y];
            }
        }
    }
    return best;
}

pointPair closestPair(const vector<xy>& points, const vector<xy>& y_sorted, int l, int r, bool fast = false){
    int sz = (r-l+1);
    if(sz <= 3){
        return closestPairBruteForce(points, l, r, fast);
    }

    int mp = (l + r) / 2; // include mp with left half
    pointPair closest_left = closestPair(points, y_sorted, l, mp, fast);
    pointPair closest_right = closestPair(points, y_sorted, mp + 1, r, fast);

    // create strip to check bipartite
    double d = min(closest_left.dist, closest_right.dist);
    // need to only consider points in l to r
    vector<xy> strip = createStrip(points, l, r, points[mp].x, d);

    // find best in strip
    pointPair closest_strip;
    for(int x{}; x < strip.size(); ++x){
        for(int offset = 1; offset <= 7 && x+offset < strip.size(); ++offset){
            if(double d = distance(strip[x], strip[x+offset], fast); d < closest_strip.dist){
                closest_strip.dist = d;
                closest_strip.p1 = new xy(strip[x]);
                closest_strip.p2 = new xy(strip[x+offset]);
            }
        }
    }

    pointPair b1 = (closest_left.dist < closest_right.dist ? closest_left : closest_right);
    pointPair best = (b1.dist < closest_strip.dist ? b1 : closest_strip);

    return best;
}

pointPair closestPair(vector<xy>& points, bool fast = false){
    // pre-wash
    sortByX(points);

    return closestPair(points, {}, 0, points.size() - 1, fast);
}

vector<xy> generateRandomPoints(int n, double lb, double rb){
    vector<xy> random_points;
    for(int x{}; x < n; ++x){
        double rx = lb + (double)rand() / RAND_MAX * (rb - lb);
        double ry = lb + (double)rand() / RAND_MAX * (rb - lb);

        random_points.push_back(xy(rx, ry));
    }
    return random_points;
}

int main(int argc, const char* argv[]){
    srand(time(NULL));

    int pts = DEFAULT_POINTS,
        lb  = DEFAULT_LOWER_BOUND,
        ub  = DEFAULT_UPPER_BOUND;
    bool use_bf = 0;
    switch(argc){
        case 1:
            break; // use default macro-initialized values
        case 2:
            pts = stoi(argv[1]);
        case 4:
            lb = stoi(argv[2]);
            ub = stoi(argv[3]);
            break;
        case 5:
            if(!strcmp(argv[4], "-bf")) use_bf = 1;
            break;
        default:
            cerr << "Usage: <num_points:Int> <lower_bound:Int> <upper_bound:Int> <-bf:Toggle Brute Force>\n";
            return 1;
    };
    printf("\nPoints: %d Lower Bound: %d Upper Bound: %d Brute-Force: %d\n", pts, lb, ub, use_bf);

    vector<xy> points = generateRandomPoints(pts, lb, ub);
    auto start_time = high_resolution_clock::now();
    auto end_time = high_resolution_clock::now();

    if(use_bf){
    {/* BRUTE FORCE */}
    start_time = high_resolution_clock::now();
    pointPair check = closestPairBruteForce(points, 0, points.size() - 1);
    end_time = high_resolution_clock::now();
    duration<double, std::milli> ms_double1 = end_time - start_time;
    cout << check << " brute force duration: " << ms_double1.count() << "\n";}

    {/* Divide and Conquer */}
    start_time = high_resolution_clock::now();
    pointPair cp = closestPair(points);
    end_time = high_resolution_clock::now();
    duration<double, std::milli> ms_double2 = end_time - start_time;
    cout << cp << " divide & conquer duration: " <<  ms_double2.count() << "\n";

    return 0;
}
