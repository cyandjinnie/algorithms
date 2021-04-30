#include <iostream>
#include <math.h>
#include <iomanip>

const double epsilon_accuracy = 0.00000001;

double square(double value) {
  return pow(value, 2);
}

class Vector3D {
 public:
  Vector3D() : x(0), y(0), z(0) {}

  Vector3D(const Vector3D& that) = default;

  Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}

  double Length() {
    return sqrt(square(x) + square(y) + square(z));
  }

  bool operator==(const Vector3D& that) const {
    return (x == that.x) && (y == that.y) && (z == that.z);
  }

  Vector3D operator-(const Vector3D& other) const {
    return Vector3D(x - other.x, y - other.y, z - other.z);
  }

  Vector3D operator+(const Vector3D& other) const {
    return Vector3D(x + other.x, y + other.y, z + other.z);
  }

  Vector3D operator*(double lambda) const {
    Vector3D temp = *this;
    temp *= lambda;
    return temp;
  }

  Vector3D operator*=(double lambda) {
    x *= lambda;
    y *= lambda;
    z *= lambda;
    return *this;
  }

  Vector3D operator/=(double lambda) {
    x /= lambda;
    y /= lambda;
    z /= lambda;
    return *this;
  }

  Vector3D operator/(double lambda) const {
    Vector3D temp = *this;
    temp /= lambda;
    return temp;
  }

  friend std::istream& operator>>(std::istream& is, Vector3D& object);

  friend Vector3D operator*(double lambda, const Vector3D& vector);

  friend double FindDistance(const Vector3D& vertex1, const Vector3D& vertex2);

 private:
  double x, y, z;
};

std::istream& operator>>(std::istream& is, Vector3D& object) {
  is >> object.x >> object.y >> object.z;
  return is;
}

Vector3D operator*(double lambda, const Vector3D& vector) {
  Vector3D temp = vector;
  temp *= lambda;
  return temp;
}

using Vertex3D = Vector3D;

class LineSegment {
 public:
  LineSegment(const Vertex3D& start, const Vertex3D& end) : start(start), end(end) {}

  Vertex3D start;
  Vertex3D end;
};

double FindDistance(const Vertex3D& vertex1, const Vertex3D& vertex2) {
  return (vertex1 - vertex2).Length();
}

double FindDistance(const Vertex3D& vertex, const LineSegment& segment) {
  if (segment.start == segment.end) {
    // Segment is degenerated into a point
    return FindDistance(vertex, segment.start);
  }

  Vertex3D left = segment.start;
  Vertex3D right = segment.end;

  while (FindDistance(left, right) > epsilon_accuracy) {
    Vertex3D m1 = left + (right - left) / 3;
    Vertex3D m2 = right - (right - left) / 3;

    if (FindDistance(vertex, m1) < FindDistance(vertex, m2)) {
      right = m2;
    } else {
      left = m1;
    }
  }

  return FindDistance(left, vertex);
}

double FindDistance(const LineSegment& segment1, const LineSegment& segment2) {
  const LineSegment& segment = segment1;

  Vertex3D left = segment2.start;
  Vertex3D right = segment2.end;

  while (FindDistance(left, right) > epsilon_accuracy) {
    Vertex3D m1 = left + (right - left) / 3;
    Vertex3D m2 = right - (right - left) / 3;

    if (FindDistance(m1, segment) < FindDistance(m2, segment)) {
      right = m2;
    } else {
      left = m1;
    }
  }

  return FindDistance(left, segment);
}

int main() {
  Vertex3D start1;
  Vertex3D end1;

  Vertex3D start2;
  Vertex3D end2;

  std::cin >> start1 >> end1 >> start2 >> end2;
  std::cout << std::setprecision(12) << FindDistance(LineSegment(start1, end1), LineSegment(start2, end2));

  return 0;
}