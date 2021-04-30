#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

//////////////////////////////////////////
//// Vector2D/Vertex2D class /////////////

// Just a handy function, improves readability
double square(double x) {
  return pow(x, 2);
}

class Vector2D {
 public:
  Vector2D() : x(0), y(0) {}
  
  Vector2D(double x, double y) : x(x), y(y) {}
  
  void Invert() {
    x = -x;
    y = -y;
  }
  
  double Length() const { return sqrt(square(x) + square(y)); }
  
  double operator*(const Vector2D& that) const {
    return x * that.x + y * that.y;
  }
  
  bool operator==(const Vector2D& that) const {
    return (x == that.x) && (y == that.y);
  }
  
  bool operator!=(const Vector2D& that) const {
    return !(*this == that);
  }
  
  Vector2D operator+=(const Vector2D& that) {
    x += that.x;
    y += that.y;
    return *this;
  }
  
  Vector2D operator+(const Vector2D& that) const {
    Vector2D copy(*this);
    copy += that;
    return copy;
  }
  
  Vector2D operator-() {
    Vector2D copy(*this);
    copy.Invert();
    return copy;
  }
  
  friend std::istream& operator>>(std::istream& is, Vector2D& object);
  
  friend std::ostream& operator<<(std::ostream& os, Vector2D& object);
 
 public:
  double x;
  double y;
};

std::istream& operator>>(std::istream& is, Vector2D& object) {
  is >> object.x >> object.y;
  return is;
}

std::ostream& operator<<(std::ostream& os, Vector2D& object) {
  os << object.x << ' ' << object.y;
  return os;
}

using Vertex2D = Vector2D;

////////////////////////////////////
//// Polygon class /////////////////

class Polygon {
 public:
  Polygon() : vertices() {}
  
  Polygon(std::vector<Vertex2D>&& vertices) : vertices(vertices) {}
  
  Polygon(const std::vector<Vertex2D>& vertices) : vertices(vertices) {}
  
  void Invert();
  
  // bool IsVertexIn(const Vertex2D& vertex) const;
  
  int StartingPointIndexMinkowsky();
  
  void ReorderVerticesWithStartingPoint();
  
  Vertex2D& operator[](size_t i) { return vertices[i]; }
  
  const Vertex2D& operator[](size_t i) const { return vertices[i]; }
  
  // vertices vector might include starting point twice
  int VerticesCount() const { return vertices.size(); }
  
  friend std::istream& operator>>(std::istream& is, Polygon& polygon);
  
 public:
  std::vector<Vertex2D> vertices;
};

std::istream& operator>>(std::istream& is, Polygon& polygon) {
  polygon.vertices.clear();
  
  int vertices_cnt = 0;
  is >> vertices_cnt;
  polygon.vertices.assign(vertices_cnt, Vector2D());
  
  if (vertices_cnt == 0) {
    return is;
  }
  
  for (int i = 0; i < vertices_cnt; ++i) {
    Vertex2D vertex;
    is >> vertex;
    polygon.vertices[vertices_cnt - (i + 1)] = vertex;
  }
  
  return is;
}

void Polygon::Invert() {
  for (int i = 0; i < vertices.size(); ++i) {
    vertices[i].Invert();
  }
}

int Polygon::StartingPointIndexMinkowsky() {
  int idx = 0;
  Vertex2D min = vertices[0];
  
  for (int i = 1; i < VerticesCount(); ++i) {
    Vertex2D current = vertices[i];
    if (current.y < min.y) {
      min = current;
      idx = i;
    } else if (current.y == min.y && current.x < min.x) {
      min = current;
      idx = i;
    }
  }
  
  return idx;
}

void Polygon::ReorderVerticesWithStartingPoint() {
  int start_idx = StartingPointIndexMinkowsky();
  std::vector<Vertex2D> new_vertices;
  
  for (int i = start_idx; i < vertices.size(); ++i) {
    new_vertices.push_back(vertices[i]);
  }
  for (int i = 0; i < start_idx; ++i) {
    new_vertices.push_back(vertices[i]);
  }
  
  vertices = std::move(new_vertices);
}

////////////////////////////////////

double PolarAngle(const Vector2D& first, const Vector2D& second) {
  double angle = atan2(second.y - first.y, second.x - first.x);
  if (angle < 0) {
    angle += 2 * M_PI;
  }
  return angle;
}

void PrepareForMinkowskySum(Polygon& polygon) {
  polygon.vertices.push_back(polygon.vertices[0]);
}

Polygon MinkowskySum(Polygon first, Polygon second) {
  Polygon result;
  result.vertices.clear();
  int i = 0;
  int j = 0;
  
  // Our data is ordered clockwise from the start, we reverse it
  // V[0] is now the starting point, downmost (1st priority) and leftmost vertex of the polygon
  first.ReorderVerticesWithStartingPoint();
  second.ReorderVerticesWithStartingPoint();
  
  // We add extra copy of the vertices[0] into the vector
  PrepareForMinkowskySum(first);
  PrepareForMinkowskySum(second);
  
  int first_size = first.VerticesCount();
  int second_size = second.VerticesCount();
  
  while (i < first_size && j < second_size) {
    Vertex2D vertex = first[i] + second[j];

#ifdef DEBUG
    std::cout << vertex << '\n';
#endif // DEBUG
    
    result.vertices.emplace_back(vertex);
    
    double first_polar_angle = PolarAngle(first[i], first[i + 1]);
    double second_polar_angle = PolarAngle(second[j], second[j + 1]);
    if (first_polar_angle < second_polar_angle) {
      ++i;
    } else if (first_polar_angle > second_polar_angle) {
      ++j;
    } else {
      ++i;
      ++j;
    }
  }
  
  // Check if we need to add extra vertex
  if (result.vertices[result.vertices.size() - 1] != result.vertices[0]) {
    result.vertices.emplace_back(first[0] + second[0]);
  }

#ifdef DEBUG
  printf("%d\n", cnt);
#endif // DEBUG
  
  return result;
}

bool PolygonsIntersect(const Polygon& first, const Polygon& second) {
  Polygon second_inverted = second;
  second_inverted.Invert();
  
  Polygon minkowsky_sum = MinkowskySum(first, second_inverted);
  
  // We check if all the vector multiplications have the same sign
  for (int i = 0; i < minkowsky_sum.VerticesCount() - 1; ++i) {
    Vertex2D current = minkowsky_sum[i];
    Vertex2D next = minkowsky_sum[i + 1];
  
    if ((next.x - current.x) * (-next.y) -
        (-next.x) * (next.y - current.y) <= 0) {
      return false;
    }
  }
  
  return true;
}

int main() {
  Polygon first;
  Polygon second;
  
  std::cin >> first >> second;
  
  if (PolygonsIntersect(first, second)) {
    std::cout << "YES";
  } else {
    std::cout << "NO";
  }
  
  return 0;
}