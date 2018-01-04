#pragma once
#include <iostream>
#include <cmath>
#include <math.h>
#include <conio.h>
#include <algorithm>
#include <iomanip>
#include "Price.h"

using namespace std;

double CumNorm(double x) {
	// protect against overflow
	if (x > 6.0)
		return 1.0;
	if (x < -6.0)
		return 0.0;
	double b1 = 0.31938153;
	double b2 = -0.356563782;
	double b3 = 1.781477937;
	double b4 = -1.821255978;
	double b5 = 1.330274429;
	double p = 0.2316419;
	double c2 = 0.3989423;
	double a = abs(x);
	double t = 1.0 / (1.0 + a * p);
	double b = c2*exp((-x)*(x / 2.0));
	double n = ((((b5*t + b4)*t + b3)*t + b2)*t + b1)*t;
	n = 1.0 - b*n;
	if (x < 0.0)
		n = 1.0 - n;
	return n;
}

class liquidityIndicator
{
public:
	liquidityIndicator() {};
	liquidityIndicator(double delta, double deltaUp, double deltaDown, double deltaStar, double alpha)
	{
		this->deltaUp = deltaUp; this->deltaDown = deltaDown; this->deltaStar = deltaStar; this->alpha = alpha; this->delta = delta;
		alphaWeight = exp(-2.0 / (alpha + 1.0)); initialized = false; surp = 1.; downSurp = 1.; upSurp = 1.; informativeness(); events = 0;
	}

	int informativeness()
	{
		H1 = -exp(-deltaStar / delta)*log(exp(-deltaStar / delta)) - (1.0 - exp(-deltaStar / delta))*log(1.0 - exp(-deltaStar / delta));
		H2 = exp(-deltaStar / delta)*pow(log(exp(-deltaStar / delta)), 2.0) - (1.0 - exp(-deltaStar / delta))*pow(log(1.0 - exp(-deltaStar / delta)), 2.0) - H1*H1;
		return 1;
		if (H1 == 0 || H2 == 0)
		{
			return 0;
		}
	}

	double liquidityEstimation(Price price)
	{
		events = intrinsicEvent(price);
		if (events != 0)
		{//Symmetric surprise
		 //cout << events << endl;
			surp = alphaWeight * (abs(events) == 1 ? 0.08338161 : 2.525729) + (1.0 - alphaWeight) * surp;

			if (events > 0) {//Downward surprises
				downSurp = alphaWeight * (events == 1 ? 0.08338161 : 2.525729) + (1.0 - alphaWeight) * downSurp;
			}
			else {//Upward surprises
				upSurp = alphaWeight * (events == -1 ? 0.08338161 : 2.525729) + (1.0 - alphaWeight) * upSurp;
			}
			//Asymmetric surprises
			liquidity = 1.0 - CumNorm(sqrt(alpha) * (surp - H1) / sqrt(H2));
			upLiquidity = 1.0 - CumNorm(sqrt(alpha) * (upSurp - H1) / sqrt(H2));
			downLiquidity = 1.0 - CumNorm(sqrt(alpha) * (downSurp - H1) / sqrt(H2));
		}
		return liquidity;
	}

	int intrinsicEvent(Price price)
	{
		if (!initialized)
		{
			type = -1; initialized = true;
			extreme = price.getmid();
			osPrice = extreme;
		}

		if (type == -1)//Downwards DC
		{
			if (price.getask() < extreme)
			{
				extreme = price.getask();
			}
			if (log(price.getbid() / extreme) >= deltaUp)
			{
				extreme = price.getask();
				osPrice = price.getask();
				type = 1;
				return 1;
			}
			if (log(osPrice / extreme) >= deltaUp)//Downard OS
			{
				osPrice = extreme;
				return 2;
			}
		}
		else if (type == 1)//Upwards DC
		{
			if (price.getbid() > extreme)
			{
				extreme = price.getbid();
			}
			if (log(price.getask() / extreme) <= -deltaDown)
			{
				extreme = price.getbid();
				osPrice = price.getbid();
				type = -1;
				return -1;
			}
			if (log(osPrice / extreme) <= -deltaDown)//Upwards OS
			{
				osPrice = extreme;
				return -2;
			}
		}
		return 0;
	}

	double getH1() { return H1; }
	double getH2() { return H2; }
	double getEvents() { return events; }
	double getSurp() { return surp; }
	double getAlphaWeight() { return alphaWeight; }
	double getUpLiq() { return upLiquidity; }
	double getDownLiq() { return downLiquidity; }
	double getLiq() { return liquidity; }

public:
	double delta, deltaUp, deltaDown, deltaStar, alpha, alphaWeight, extreme, osPrice,
		surp, downSurp, upSurp, H1, H2, liquidity, upLiquidity, downLiquidity;
	int events, type;
	bool initialized;
};



