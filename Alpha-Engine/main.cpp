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
#include "AlphaEnginePanel.h"
#include "CSVIterator.h"
#include "GBM.h"
#include "LinkedList.h"


using namespace std;

static int cumdays[] = { 0, 0,31,59,90,120,151,181,212,243,273,304,334 };
double datenum(int year, int mon, int day, int hour, int imin, int sec, int mil)
{
	int tmp1, tmp2, tmp3;
	double	tmp4, tmp5;
	double dNum;

	/* Calculate the serial date number:*/
	tmp1 = 365 * year + cumdays[mon] + day;
	tmp2 = year / 4 - year / 100 + year / 400;
	tmp3 = (year % 4 != 0) - (year % 100 != 0) + (year % 400 != 0);
	tmp4 = (double)(tmp1 + tmp2 + tmp3);
	tmp5 = (hour * 3600000 + imin * 60000 + sec * 1000 + mil) / 86400000.0;

	dNum = tmp4 + tmp5;

	if (mon > 2) {
		if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0)) {
			dNum += 1.0;
		}
	}

	return(dNum);
}

int main()
{

	ifstream Data;
	Price Rate;
	float ask, bid;
	//Data.open("btceUSD.csv");
	Data.open("NZDJPY_Ticks_2006.01.01_2013.12.31.csv");
	AlphaEnginePanel alphaEngine(10000.0);
	ofstream Profit;
	ofstream DateNum;
	ofstream Inventory;
	//DateNum.open("DateNum.csv");
	//DateNum << "Time (UTC)" << endl;
	
	Profit.open("Profit.csv");
	Profit << "Ask," << "Bid," << "Profit," << endl;
	//Inventory.open("Inventory.csv");
	string date;
	int year, month, day, hour, minute, seconds, milliseconds;
	//vector<string> wordVector;
	double dateNum = 0.0;
	try 
	{
		int i = 0;
		for (CSVIterator loop(Data); loop != CSVIterator(); loop++)
		{
			ask = ::atof((*loop)[1].c_str());
			bid = ::atof((*loop)[2].c_str());
			Rate = Price(bid, ask);
			alphaEngine.run(Rate);
			
			//cout << "Step " << i;
			//cout << " total PnL: " << alphaEngine.getGlobalPnl(Rate) << endl;
			//Inventory << alphaEngine.getGlobalInventory() << endl;
			Profit << ask << ","; Profit << bid << ",";
			Profit << alphaEngine.getGlobalPnl(Rate) << endl;
			i++;

		}
	}
	catch (const exception &e) {}
	Profit.close();
	//Inventory.close();
	getchar();
	return 0;
	
	/*
	string date; 
	vector<string> wordVector;
	int year, month, day, hour, minute, seconds, milliseconds;
	double dateNum;
	try
	{
		for (CSVIterator loop(Data); loop != CSVIterator(); loop++)
		{
			date = (*loop)[0];
			if (date != "Time (UTC)")
			{
				std::stringstream stringStream(date);
				std::string line;
				while (std::getline(stringStream, line))
				{
					std::size_t prev = 0, pos;
					while ((pos = line.find_first_of(" .:", prev)) != std::string::npos)
					{
						if (pos > prev)
							wordVector.push_back(line.substr(prev, pos - prev));
						prev = pos + 1;
					}
					if (prev < line.length())
						wordVector.push_back(line.substr(prev, std::string::npos));
				}
				year = ::atoi(wordVector[0].c_str()); month = ::atoi(wordVector[1].c_str()); day = ::atoi(wordVector[2].c_str());
				hour = ::atoi(wordVector[3].c_str()); minute = ::atoi(wordVector[4].c_str()); seconds = ::atoi(wordVector[5].c_str()); milliseconds = ::atoi(wordVector[6].c_str());
				dateNum = datenum(year, month, day, hour, minute, seconds, milliseconds);
				DateNum << setprecision(25) << dateNum << endl;
				date = ""; wordVector.clear();
			}
		}
	}
	catch (const exception &e) {}
	DateNum.close();
	getchar();
	return 0;
	*/
	/*
	int nSimulations = 60;
	long nPoints = 10000000L;
	const long numResults = 1000;
	int saveEvery = (int)(nPoints / numResults);
	double averagePnL[int(numResults)] = {};
	try {
		for (int iSimul = 0; iSimul < nSimulations; iSimul++)
		{
			AlphaEnginePanel alphaEngine = AlphaEnginePanel(1000.0);
			GBM gbm = GBM(1.0f, 0.40f, 1.0f, nPoints, 0);

			for (long i = 0L; i < nPoints; i++) {
				double generatedValue = gbm.monteCarloSimulation();
				Price price = Price((float)generatedValue, (float)generatedValue);
				alphaEngine.run(price);

				if (i % saveEvery == 0) {
					cout << "Step " << i << ", total PnL: " << alphaEngine.getGlobalPnl(price) << endl;
					averagePnL[(int)(i / saveEvery)] += alphaEngine.getGlobalPnl(price);
				}
			}		
		}
	}
	catch(exception &e) {}


	for (int index = 0; index < numResults; index++) 
	{
		averagePnL[index] = averagePnL[index] / nSimulations;
		cout << (averagePnL[index]) << endl;
	}
	getchar();
	return 0;
	*/
}
