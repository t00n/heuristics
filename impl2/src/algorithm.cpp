#include "algorithm.hpp"

Algorithm::Algorithm(const PfspInstance & instance) {
	this->instance = instance;
}

PfspSolution Algorithm::getSolution() {
	return this->solution;
}