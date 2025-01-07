#ifndef HASWGT_HPP
#define HASWGT_HPP

#include <vector>
#include <array>
#include <memory>

// Class that represents an object with an array of weights (wgtCent) and a counter (count)
// The class is a template and can be used with different data types (PT) and different sizes (PD)
template <typename PT, std::size_t PD>
class HasWgtCent {
public:
    std::array<PT, PD> wgtCent; // Array of weights, the size is determined by the template parameter PD
    int count = 0;               // A counter associated with the object (e.g., to count operations)

    // Default constructor
    // Initializes wgtCent with all zero values and count to zero
    HasWgtCent() : wgtCent(), count(0) {
        wgtCent.fill(0);  // Set all elements of wgtCent to zero
    }


    // Method to reset the weights and the counter
    // Sets all elements of wgtCent to zero and count to zero
    void resetCount() {
        wgtCent.fill(0);  // Reset the weights
        count = 0;        // Reset the counter
    }

    // Virtual default destructor
    // Since the class may be inherited, the destructor should be virtual to ensure proper destruction of derived objects
    virtual ~HasWgtCent() = default;
};

#endif
