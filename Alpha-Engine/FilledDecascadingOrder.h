#pragma once
#include <iostream>

using namespace std;

class FilledDecascadingOrder
{
private:
	int longShort;
	double deltaPrice, cascadingLevel, decascadingLevel;
	int volume;
public:
	FilledDecascadingOrder(int longShort, int volume, double cascadingLevel, double decascadingLevel)
	{
		this->longShort = longShort; this->volume = volume; this->cascadingLevel = cascadingLevel;
		this->decascadingLevel = decascadingLevel; this->deltaPrice = (decascadingLevel - cascadingLevel) * longShort;
	}

	int getLongShort()
	{
		return longShort;
	}

	double getDeltaPrice()
	{
		return deltaPrice;
	}

	int getVolume()
	{
		return volume;
	}

	double getCascadingLevel()
	{
		return cascadingLevel;
	}

	double getDecascadingLevel()
	{
		return decascadingLevel;
	}
};