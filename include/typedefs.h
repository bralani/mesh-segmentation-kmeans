// typedefs.h
#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <functional>
#include <vector>

typedef std::function<double(const std::vector<double>&, const std::vector<double>&)> DistanceMetric;

#endif // TYPEDEFS_H
