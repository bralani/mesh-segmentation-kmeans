#ifndef ENTRY_HPP
#define ENTRY_HPP

#include <memory>
#include <string>

/*
 * The entry to book Cut Discrepancy results
 */
struct EntryCD {
    double CD = 0.0;
};

/*
 * The entry to book Consistency Error results
 */
struct EntryCE {
    double GCE = 0.0;
    double LCE = 0.0;
    double GCEa = 0.0;
    double LCEa = 0.0;
};

/*
 * The entry to book Hamming Distance
 */
struct EntryHD {
    double distance = 0.0;
    double missingRate = 0.0;
    double falseAlarmRate = 0.0;
};

/*
 * The entry to book Rand Index related measure
 */
struct EntryRI {
    double RI = 0.0; // Rand Index
};

/*
 * The entry to book all evaluations
 */
struct Entry {
    std::string name;                                // Name of the entry
    std::unique_ptr<EntryCE> eCE = nullptr;         // Consistency Error
    std::unique_ptr<EntryHD> eHD = nullptr;         // Hamming Distance
    std::unique_ptr<EntryCD> eCD = nullptr;         // Cut Discrepancy
    std::unique_ptr<EntryRI> eRI = nullptr;         // Rand Index
};

#endif // ENTRY_HPP
