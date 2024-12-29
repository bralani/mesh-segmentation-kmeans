#ifndef ENTRY_HPP
#define ENTRY_HPP

#include <memory>
#include <string>

/*
 * Entry.hpp
 * 
 * Define the structs to book evaluations results
 * 
 */

/*
 * The entry to book Cut Discrepancy results
 */
struct Entry_CD {
    double CD = 0;

    friend std::ostream& operator<<(std::ostream& os, const Entry_CD& entry) {
        os << "Evaluation of Cut Discrepancy ...\n";
        os << "CD: " << entry.CD;
        os << std::endl;
        return os;
    }
};

/*
 * The entry to book Consistency Error results
 */
struct Entry_CE {
    double GCE = 0;
    double LCE = 0;
    double GCEa = 0;
    double LCEa = 0;

    friend std::ostream& operator<<(std::ostream& os, const Entry_CE& entry) {
        os << "Evaluation of Consistency Error ...\n";
        os << "GCE: " << entry.GCE << ", LCE: " << entry.LCE
           << ", GCEa: " << entry.GCEa << ", LCEa: " << entry.LCEa;
        os << std::endl;
        return os;
    }
};

/*
 * The entry to book Hamming Distance
 */
struct Entry_HD {
    double distance = 0;
    double missingRate = 0;
    double falseAlarmRate = 0;

    friend std::ostream& operator<<(std::ostream& os, const Entry_HD& entry) {
        os << "Evaluation of Hamming Distance ...\n";
        os << "Distance: " << entry.distance
           << ", Missing Rate: " << entry.missingRate
           << ", False Alarm Rate: " << entry.falseAlarmRate;
        os << std::endl;
        return os;
    }
};

/*
 * The entry to book Rand Index related measure
 */
struct Entry_RI {
    double RI = 0; // Rand Index

    friend std::ostream& operator<<(std::ostream& os, const Entry_RI& entry) {
        os << "Evaluation of Rand Index ...\n";
        os << "RI: " << entry.RI;
        os << std::endl;
        return os;
    }
};

#endif // ENTRY_HPP
