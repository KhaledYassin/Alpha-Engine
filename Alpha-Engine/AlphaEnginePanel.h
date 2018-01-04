#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include <conio.h>
#include <algorithm>
#include <random>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include "Price.h"
#include "LimitOrder.h"
#include "CoastlineTrader.h"

class AlphaEnginePanel
{
public:
	AlphaEnginePanel() {};

	AlphaEnginePanel(double initialWealth)
	{
		this->initialWealth = initialWealth; globalProfit = 0.0;
		initiateTraders(); globalInventory = 0;
	}

	void initiateTraders()
	{
		longCoastlineTraders.push_back(CoastlineTrader(0.0025, 0.0025, 0.0025, 1));
		longCoastlineTraders.push_back(CoastlineTrader(0.005, 0.005, 0.005, 1));
		longCoastlineTraders.push_back(CoastlineTrader(0.01, 0.01, 0.01, 1));
		longCoastlineTraders.push_back(CoastlineTrader(0.015, 0.015, 0.015, 1));

		shortCoastlineTraders.push_back(CoastlineTrader(0.0025, 0.0025, 0.0025, -1));
		shortCoastlineTraders.push_back(CoastlineTrader(0.005, 0.005, 0.005, -1));
		shortCoastlineTraders.push_back(CoastlineTrader(0.01, 0.01, 0.01, -1));
		shortCoastlineTraders.push_back(CoastlineTrader(0.015, 0.015, 0.015, -1));
	}

	void run(Price price)
	{
		for (int i = 0; i < longCoastlineTraders.size(); i++)
		{
			longCoastlineTraders[i].run(price);
		}
		for (int i = 0; i < shortCoastlineTraders.size(); i++)
		{
			shortCoastlineTraders[i].run(price);
		}
	}

	double getGlobalPnl(Price price)
	{
		globalProfit = 0.0;
		for (int i = 0; i < longCoastlineTraders.size(); i++)
		{
			globalProfit += longCoastlineTraders[i].getRealizedProfit();
			globalProfit += longCoastlineTraders[i].getPositionProfit(price);
		}
		for (int i = 0; i < shortCoastlineTraders.size(); i++)
		{
			globalProfit += shortCoastlineTraders[i].getRealizedProfit();
			globalProfit += shortCoastlineTraders[i].getPositionProfit(price);
		}
		return globalProfit / initialWealth;
	}

	int getGlobalInventory()
	{
		globalInventory = 0;
		for (int i = 0; i < longCoastlineTraders.size(); i++)
		{
			globalInventory += longCoastlineTraders[i].getInventory();
		}
		for (int i = 0; i < shortCoastlineTraders.size(); i++)
		{
			globalInventory += shortCoastlineTraders[i].getInventory();
		}
		return globalInventory;
	}

private:
	vector<CoastlineTrader> longCoastlineTraders;
	vector<CoastlineTrader> shortCoastlineTraders;
	double globalProfit, initialWealth;
	int globalInventory;

};