#ifndef HASWGT_HPP
#define HASWGT_HPP

#include <vector>
#include <array>
#include <memory>


template <typename PT, std::size_t PD>
class HasWgtCent {
public:
    std::array<PT, PD> wgtCent; 
    int count = 0;         

    HasWgtCent() : wgtCent(), count(0) {
        wgtCent.fill(0);
    }

    virtual ~HasWgtCent() = default;

    void resetCount() {
        wgtCent.fill(0);
        count = 0;
    }

};

#endif