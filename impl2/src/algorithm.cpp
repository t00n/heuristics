#include "algorithm.hpp"

Algorithm::Algorithm(const PfspInstance & instance, std::time_t timeout) {
	this->instance = instance;
	this->timeout = timeout;
}

PfspSolution Algorithm::getSolution() {
	return this->solution;
}