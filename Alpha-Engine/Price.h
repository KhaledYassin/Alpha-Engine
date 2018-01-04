#pragma once
#include <iostream>
using namespace std;

class Price
{
private:
	float bid, ask;
public:
	Price() {};
	Price(float bid, float ask) { this->bid = bid; this->ask = ask; }
	Price clone()
	{
		Price Original(bid, ask); Price Copy(Original);
		return Copy;
	}

	float getbid() { return bid; }
	float getask() { return ask; }
	float getmid() { return (bid + ask) / 2.; }
	float getSpread() { return ask - bid; }
};
