#include "Payment.h"
using namespace std;
PricingStrategy* Payment::strategy = nullptr;

void Payment::setStrategy(PricingStrategy* newStrategy) {
    strategy = newStrategy;
}

double Payment::calculateFee(std::time_t entryTime) {
    if (strategy == nullptr) {
        // default fallback
        time_t now = time(nullptr);
        double hours = difftime(now, entryTime) / 3600.0;
        return hours * 20.0;
    }
    return strategy->calculateFee(entryTime);
}