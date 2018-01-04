#pragma once
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <algorithm>
#include <random>

using namespace std;

class GBM
{
public:
	GBM() {};
	GBM(float initialValue, float sigma, float nYears, long nGenerations, float miu)
	{
		process = initialValue; this->sigma = sigma; this->miu = miu;
		deltaT = nYears / nGenerations; initiated = false;
	}

	float monteCarloSimulation()
	{
		if (!initiated)
		{
			initiated = true;
		}
		else
		{
			static mt19937 rng; //Mersenne Twister random number generator
			normal_distribution<> ND(0., 1.);//Gaussian Standard Normal Distribution with a mean of zero and variance of 1 
			phi = ND(rng);
			process += process * (miu*deltaT + sigma*sqrt(deltaT)*phi);
		}
		return process;
	}

private:
	float sigma, miu, deltaT, process, phi;
	bool initiated;

};