#pragma once
#include <iostream>
#include "Price.h"
#include <forward_list>
using namespace std;

class LimitOrder
{
public:
	int type, dcORos, mode;
	Price priceOpened;
	int volume;
	double level, delta;
	forward_list<LimitOrder> compensatedOrders;
public:

	LimitOrder() {};
	LimitOrder(int type, Price priceOpened, double level, int volume, double delta, int dcORos, int mode)
	{
		this->type = type; this->priceOpened = priceOpened; this->level = level; this->volume = volume;
		this->delta = delta; this->dcORos = dcORos; this->mode = mode;
		this->compensatedOrders.clear();
	}

	LimitOrder clone()
	{
		LimitOrder Original(type, priceOpened, level, volume, delta, dcORos, mode);
		LimitOrder Copy(Original);
		return Copy;
	}

	void setLevel(double level) { this->level = level; }

	int getType() { return type; }

	Price getPriceOpened() { return priceOpened; }

	double getLevel() { return level; }

	int getVolume() { return volume; }

	int getDcRos() { return dcORos; }

	double getDelta() { return delta; }

	void changeVolume(double newVolume) { volume = newVolume; }

	void addCompensatedOrder(LimitOrder compensatedOrder) { compensatedOrders.push_front(compensatedOrder); }

	void cleanCompensatedOrder() { compensatedOrders.clear(); }

	forward_list<LimitOrder> getCompensatedOrders() { return compensatedOrders; }

};