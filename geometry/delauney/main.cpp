#include <iostream>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <map>
#include <set>

double square(double x) { return pow(x, 2); }

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
 * @return std::vector<Event*> movie
*/
Movie ConvexHullImpl(std::vector<Point>& points, int left, int right) {

}

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
  
  // 2-d kinetic movie output
  std::vector<Event*> movie;
  
  movie = LowerHull(vector, 0, size);
  GetFacets(movie, facets, Point::EventType::Deletion);
  
  // Drop links, so that we start from scratch
  UnlinkPoints(vector);
  
  //movie = UpperHull(vector, 0, size);
  //GetFacets(movie, facets, Point::EventType::Insertion);
  
  std::sort(facets.begin(), facets.end());
  
  return facets;
}

////////////////////////////////////
///// Voronoi //////////////////////

struct Point2D {
  double x = 0;
  double y = 0;
  int index = -1;
};

std::istream& operator>>(std::istream& is, Point2D& point) {
  is >> point.x >> point.y;
  return is;
}

double MeanVoronoiPolygonEdges(const std::vector<Point2D>& vertices) {
  std::vector<Point> vertices3d;
  
  for (auto& vertex2d : vertices) {
    double x = vertex2d.x;
    double y = vertex2d.y;
    
    // z = x^2 + y^2
    Point vertex3d(x, y, square(x) + square(y));
    vertex3d.index = vertex2d.index;
    vertex3d.ApplyRotation(1e-8);
    
    vertices3d.emplace_back(vertex3d);
  }
  std::vector<Facet> facets = ConvexHull(vertices3d);

#ifdef DEBUG
  for (auto& facet : facets) {
    std::cout << facet.i << ' ' << facet.j << ' ' << facet.k << '\n';
  }
  std::cout << '\n';
#endif // DEBUG
  
  std::map<std::pair<int, int>, int> edge_count;
  std::vector<int> vertex_degree(vertices.size(), 0);
  std::vector<bool> is_outer_vertex(vertices.size(), false);
  
  for (Facet& facet : facets) {
    edge_count[std::minmax(facet.i, facet.j)]++;
    edge_count[std::minmax(facet.j, facet.k)]++;
    edge_count[std::minmax(facet.i, facet.k)]++;
    
    vertex_degree[facet.i] += 2;
    vertex_degree[facet.j] += 2;
    vertex_degree[facet.k] += 2;
  }
  
  for (Facet& facet : facets) {
    if (edge_count[std::minmax(facet.i, facet.j)] % 2 != 0) {
      is_outer_vertex[facet.i] = true;
      is_outer_vertex[facet.j] = true;
    }
    
    if (edge_count[std::minmax(facet.j, facet.k)] % 2 != 0) {
      is_outer_vertex[facet.j] = true;
      is_outer_vertex[facet.k] = true;
    }
    
    if (edge_count[std::minmax(facet.i, facet.k)] % 2 != 0) {
      is_outer_vertex[facet.i] = true;
      is_outer_vertex[facet.k] = true;
    }
    
  }
  
  int num_polygon_edges = 0;
  int num_polygons = 0;
  
  for (int cur_vertex = 0; cur_vertex < vertices.size(); ++cur_vertex) {
    if (!is_outer_vertex[cur_vertex]) {
      num_polygon_edges += vertex_degree[cur_vertex] / 2;
      ++num_polygons;
    }
  }
  
  return num_polygons != 0 ? (double)num_polygon_edges / num_polygons : 0;
}

int32_t main() {
  std::vector<Point2D> points;
  
  int i = 0;
  Point2D current;
  while (std::cin >> current) {
    current.index = i;
    ++i;
    points.emplace_back(current);
  }
  
  std::cout << std::setprecision(12) << MeanVoronoiPolygonEdges(points) << '\n';
  
  return 0;
}