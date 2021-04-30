#include <iostream>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <vector>

/////////////////////////////////////////
////////// Point ////////////////////////

class Point {
 public:
  explicit Point(double x = 0, double y = 0, double z = 0, int index = -1,
                 Point* next = nullptr, Point* prev = nullptr) noexcept
    : x(x), y(y), z(z), index(index), next(next), prev(prev) {}
 
 public:
  friend bool operator<(const Point& left, const Point& right);
  
  friend std::istream& operator>>(std::istream& is, Point& point);
  
  enum class EventType {
    Insertion,
    Deletion
  };
  
  EventType TriggerEvent() {
    if (prev->next != this) {
      // Insertion
      prev->next = next->prev = this;
      return EventType::Insertion;
    } else {
      // Deletion
      prev->next = next;
      next->prev = prev;
      return EventType::Deletion;
    }
  }
  
  void InvertZAxis() {
    z = -z;
  }
  
  void ApplyRotation(double phi) {
    double _x = 0;
    double _y = 0;
    double _z = 0;
    
    // Rotate XY
    _x = x * cos(phi) - y * sin(phi);
    _y = x * sin(phi) + y * cos(phi);
    
    x = _x;
    y = _y;
    
    // Rotate XZ
    
    _x = x * cos(phi) - z * sin(phi);
    _z = x * sin(phi) + z * cos(phi);
    
    x = _x;
    z = _z;
    
    // Rotate YZ
  
    _y = y * cos(phi) - z * sin(phi);
    _z = y * sin(phi) + z * cos(phi);
    
    y = _y;
    z = _z;
  }
  
 public:
  double x;
  double y;
  double z;
  int index;
  
  Point* next = nullptr;
  Point* prev = nullptr;
};

using Event = Point;

// less operator for std::sort
bool operator<(const Point& left, const Point& right) {
  return left.x < right.x;
}

std::istream& operator>>(std::istream& is, Point& point) {
  is >> point.x >> point.y >> point.z;
  return is;
}

std::ostream& operator<<(std::ostream& os, Point& point) {
  os << point.x << ' ' << point.y << ' ' << point.z;
  return os;
}

double const INF = 1e50;

//static Point NullEventObject(INF, INF, INF, nullptr, nullptr);
//Point const* NullEvent = &NullEventObject;

// todo:
void rotate_point(double angle, Point& point);

//////////////////////////////////////////
//////// Facet class /////////////////////

class Facet {
 public:
  Facet(int i, int j, int k) : i(i), j(j), k(k) {}
 
  friend std::ostream& operator<<(std::ostream& os, Facet& facet);
  
  void OutputFormat() {
    if (j < k && j < i) {
      std::swap(i, j);
      std::swap(j, k);
    } else if (k < i && k < j) {
      std::swap(i, k);
      std::swap(j, k);
    }
  }
  
 public:
  int i;
  int j;
  int k;
};

std::ostream& operator<<(std::ostream& os, Facet& facet) {
  os << facet.i << ' ' << facet.j << ' ' << facet.k;
}

bool operator<(const Facet& left, const Facet& right) {
  if (left.i < right.i) {
    return true;
  } else if (left.i == right.i && left.j < right.j) {
    return true;
  } else if (left.i == right.i && left.j == right.j && left.k < right.k) {
    return true;
  }
  
  return false;
}

/////////////////////////////////////////
////////////// Convex hull //////////////

inline double Turn(Point* first, Point* second, Point* third) {
  if (first == nullptr || second == nullptr || third == nullptr) {
    return INF;
  } else {
    // We multiply projections in Oxy
    return (second->x - first->x) * (third->y - second->y)
            - (second->y - first->y) * (third->x - second->x);
  }
}

inline bool ClockwiseTurn(Point* first, Point* second, Point* third) {
  return Turn(first, second, third) < 0;
}

double OrientationSwitchTime(Point* first, Point* second, Point* third) {
  if (first == nullptr || second == nullptr || third == nullptr) {
    return INF;
  }
  return ((second->x - first->x) * (third->z - second->z) - (second->z - first->z) * (third->x - second->x)) / Turn(first, second, third);
}

///////////////////////////////////////////
///////// Convex hull /////////////////////

using Movie = std::vector<Event*>;

inline int Index(Point* point) {
  return point->index;
}

/*
 * Builds 2-d kinetic movie from points[left, right)
 * @return std::vector<Event*> movie of the lower hull
*/
Movie LowerHull(std::vector<Point>& points, int left, int right) {
  if (right - left == 1) {
    // Single element
    return std::vector<Event*>();
  }
  
  int mid = (left + right) / 2;
  
  Movie left_lower_hull = LowerHull(points, left, mid);
  Movie right_lower_hull = LowerHull(points, mid, right);
  Movie result = std::vector<Event*>();
  
  // uv is a bridge between left and right 2-d kinetic hull
  Point* u = &points[mid - 1];
  Point* v = &points[mid];
  
  // Find initial bridge at t = -inf
  while (true) {
    if (ClockwiseTurn(u, v, v->next)) {
      v = v->next;
    } else if (ClockwiseTurn(u->prev, u, v)) {
      u = u->prev;
    } else {
      break;
    }
  }
  
  int left_index = 0;
  int right_index = 0;
  double cur_time = -INF;
  double new_time = -INF;
  
  for (cur_time = -INF;;) {
    std::vector<double> t(6, INF);
    
    /* vector `t` stores time of following events:
     * t[0] - insertion/deletion from left lower hull
     * t[1] - insertion/deletion from right lower hull
     * t[2] - u->prev, u, v shift, and thus deletion
       of `u` and
     * t[3] - u, u->next, v shift, and thus insertion
       if `u->next`
     * t[4] - u, v->prev, v shift, and thus insertion
       if `v->prev`
     * t[5] - u, v, v->next shift, and thus deletion
       if `v`
    */
    
    Point* left_ptr = nullptr;
    Point* right_ptr = nullptr;
    
    if (left_index < left_lower_hull.size()) {
      left_ptr = left_lower_hull[left_index];
      t[0] = OrientationSwitchTime(left_ptr->prev, left_ptr, left_ptr->next);
    }
    
    if (right_index < right_lower_hull.size()) {
      right_ptr = right_lower_hull[right_index];
      t[1] = OrientationSwitchTime(right_ptr->prev, right_ptr, right_ptr->next);
    }
    
    t[2] = OrientationSwitchTime(u->prev, u, v);
    t[3] = OrientationSwitchTime(u, u->next, v);
    t[4] = OrientationSwitchTime(u, v->prev, v);
    t[5] = OrientationSwitchTime(u, v, v->next);
    
    // Search for event across these six for the closest one
    int closest_event_idx = -1;
    double min_event_time = INF;
    
    for (int i = 0; i < 6; ++i) {
      if (t[i] > cur_time && t[i] < min_event_time) {
        min_event_time = t[i];
        closest_event_idx = i;
      }
    }
    
    if (closest_event_idx == -1 || min_event_time >= INF) {
      break;
    }
    
    switch (closest_event_idx) {
      case 0:
        if (left_ptr->x < u->x) {
          result.push_back(left_ptr);
        }
        left_ptr->TriggerEvent();
        ++left_index;
        break;
      case 1:
        if (right_ptr->x > v->x) {
          result.push_back(right_ptr);
        }
        right_ptr->TriggerEvent();
        ++right_index;
        break;
      case 2:
        result.push_back(u);
        u = u->prev;
        break;
      case 3:
        u = u->next;
        result.push_back(u);
        break;
      case 4:
        v = v->prev;
        result.push_back(v);
        break;
      case 5:
        result.push_back(v);
        v = v->next;
        break;
      default:
        abort();
    }
    
    cur_time = min_event_time;
  }
  
  u->next = v;
  v->prev = u;
  
  // Update pointers in inverse chronology
  // can be thought of as a rewind of a movie
  for (ssize_t i = result.size() - 1; i >= 0; --i) {
    Point* current_event = result[i];
    
    if (current_event->x <= u->x || current_event->x >= v->x) {
      current_event->TriggerEvent();
      if (current_event == u) {
        u = u->prev;
      } else if (current_event == v) {
        v = v->next;
      }
    } else {
      // Deleted/inserted point inside (u.x, v.x)
      u->next = current_event;
      current_event->prev = u;
      current_event->next = v;
      v->prev = current_event;
      
      if (current_event->x <= points[mid - 1].x) {
        u = current_event;
      } else {
        v = current_event;
      }
    }
  }
  
  return result;
}

void FlipMeshZAxis(std::vector<Point>& points) {
  for (Point& point : points) {
    point.InvertZAxis();
  }
}

void UnlinkPoints(std::vector<Point>& points) {
  for (Point& point : points) {
    point.next = nullptr;
    point.prev = nullptr;
  }
}

inline Movie UpperHull(std::vector<Point>& points, int left, int right) {
  FlipMeshZAxis(points);
  return LowerHull(points, left, right);
}

void GetFacets(const Movie& movie, std::vector<Facet>& facets, Point::EventType parity) {
  for (Event* event : movie) {
    Facet facet(Index(event->prev), Index(event), Index(event->next));
    
    if (event->TriggerEvent() == parity) {
      std::swap(facet.i, facet.j);
    }
    
    facet.OutputFormat();
    facets.emplace_back(facet);
  }
}

/*
 * Returns facets of the original point vector
 */
std::vector<Facet> ConvexHull(std::vector<Point> vector) {
  std::sort(vector.begin(), vector.end());
  int size = vector.size();
  std::vector<Facet> facets;

#ifdef DEBUG
  for (auto&& point : vector) {
    std::cout << point.x << ' ' << point.y << ' ' << point.z << '\n';
  }
#endif // DEBUG
  
  // 2-d kinetic movie output
  std::vector<Event*> movie;
  
  movie = LowerHull(vector, 0, size);
  GetFacets(movie, facets, Point::EventType::Deletion);
  
  // Drop links, so that we start from scratch
  UnlinkPoints(vector);
  
  movie = UpperHull(vector, 0, size);
  GetFacets(movie, facets, Point::EventType::Insertion);
  
  std::sort(facets.begin(), facets.end());
  
  return facets;
}

int main() {
  int num_tests = 0;
  std::cin >> num_tests;
  
  for (int test_no = 0; test_no < num_tests; ++test_no) {
    int N = 0;
    std::cin >> N;
  
    std::vector<Point> vertices(N);
  
    for (int i = 0; i < N; ++i) {
      std::cin >> vertices[i];
      vertices[i].index = i;
      vertices[i].ApplyRotation(1e-2);
    }
  
    std::vector<Facet> hull = ConvexHull(vertices);
  
    std::cout << hull.size() << '\n';
    for (auto &&face : hull) {
      std::cout << "3 " << face.i << ' ' << face.j << ' ' << face.k << '\n';
    }
  }
  
  return 0;
}