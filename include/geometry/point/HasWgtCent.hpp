#ifndef HASWGT_HPP
#define HASWGT_HPP

#include <vector>
#include <array>
#include <memory>

/**
 * \class HasWgtCent
 * \brief Represents an object with an array of weights and a counter.
 * 
 * This class template provides a structure to hold an array of weights (`wgtCent`) 
 * and a counter (`count`). The size of the array and the data type for weights 
 * are determined by the template parameters `PT` (the type of weights) and `PD` 
 * (the number of elements in the array). The class also includes a method for 
 * resetting the weights and counter to their initial values.
 * 
 * \tparam PT The data type of the weights (e.g., float, double).
 * \tparam PD The size of the weight array (i.e., number of dimensions or elements).
 */
template <typename PT, std::size_t PD>
class HasWgtCent {
public:
    /**
     * \brief Array of weights.
     * 
     * This array holds the weights, and its size is determined by the template 
     * parameter `PD`. All elements in this array are initialized to zero by 
     * the default constructor.
     */
    std::array<PT, PD> wgtCent;

    /**
     * \brief Counter associated with the object.
     * 
     * The counter is initialized to zero by the default constructor. This counter 
     * can be used to track the number of operations or updates performed on 
     * the object.
     */
    int count = 0;

    /**
     * \brief Default constructor.
     * 
     * Initializes the `wgtCent` array with all zero values and the `count` to zero.
     */
    HasWgtCent() : wgtCent(), count(0) {
        wgtCent.fill(0);  ///< Set all elements of wgtCent to zero.
    }

    /**
     * \brief Resets the weights and counter.
     * 
     * This method resets the `wgtCent` array to all zero values and sets the `count` 
     * back to zero. It is useful for clearing or reinitializing the object.
     */
    void resetCount() {
        wgtCent.fill(0);  ///< Reset the weights to zero.
        count = 0;        ///< Reset the counter to zero.
    }

    /**
     * \brief Virtual default destructor.
     * 
     * The destructor is virtual, ensuring that if the class is inherited, the 
     * derived class’s destructor is called correctly when an object of the derived 
     * type is destroyed.
     */
    virtual ~HasWgtCent() = default;
};

#endif
