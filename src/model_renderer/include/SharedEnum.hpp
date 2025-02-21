#ifndef ENUMS_HPP
#define ENUMS_HPP

#include <string>

class Enums
{
public:
    enum class KInit
    {
        ELBOW_METHOD,
        KDE_METHOD,
        SILHOUETTE_METHOD
    };

    enum class CentroidInit
    {
        RANDOM,
        KDE,
        MOSTDISTANT,
        KDE3D
    };

    enum class MetricMethod
    {
        EUCLIDEAN,
        DIJKSTRA,
        HEAT
    };

    static std::string toString(KInit kInit)
    {
        switch (kInit)
        {
        case KInit::ELBOW_METHOD:
            return "Elbow Method";
        case KInit::KDE_METHOD:
            return "KDE Method";
        case KInit::SILHOUETTE_METHOD:
            return "Silhouett Method";
        default:
            return "Unknown KInit Method";
        }
    }

    static std::string toString(CentroidInit centroidInit)
    {
        switch (centroidInit)
        {
        case CentroidInit::RANDOM:
            return "Random";
        case CentroidInit::KDE:
            return "Kernel Density Estimator";
        case CentroidInit::MOSTDISTANT:
            return "Most Distant";
        case CentroidInit::KDE3D:
            return "Static KDE - 3D Point";
        default:
            return "Unknown Centroid Init Method";
        }
    }

    static std::string toString(MetricMethod metricMethod)
    {
        switch (metricMethod)
        {
        case MetricMethod::DIJKSTRA:
            return "Dijkstra";
        case MetricMethod::EUCLIDEAN:
            return "Euclidean";
        case MetricMethod::HEAT:
            return "Heat";
        default:
            return "Unknown Metric Method";
        }
    }
};

// Overload operator== for CentroidInit and int
inline bool operator==(Enums::CentroidInit centroidInit, int value)
{
    return static_cast<int>(centroidInit) == value;
}

inline bool operator==(int value, Enums::CentroidInit centroidInit)
{
    return value == static_cast<int>(centroidInit);
}

// Overload operator== for CentroidInit and int
inline bool operator==(Enums::KInit kinit, int value)
{
    return static_cast<int>(kinit) == value;
}

inline bool operator==(int value, Enums::KInit kinit)
{
    return value == static_cast<int>(kinit);
}

// Overload operator== for CentroidInit and int
inline bool operator==(Enums::MetricMethod kinit, int value)
{
    return static_cast<int>(kinit) == value;
}

inline bool operator==(int value, Enums::MetricMethod kinit)
{
    return value == static_cast<int>(kinit);
}

#endif // ENUMS_HPP
