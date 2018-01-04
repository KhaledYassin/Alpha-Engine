#pragma once
#include <iostream>
#include <cmath>
#include <math.h>
#include <conio.h>
#include <algorithm>
#include <iomanip>
#include <vector>
#include "Price.h"
#include "LiquidityIndicator.h"
#include "LimitOrder.h"
#include "LinkedList.h"
#include <array>
#include <forward_list>
using namespace std;

class CoastlineTrader
{
public:
	CoastlineTrader() {};
	CoastlineTrader(double originalDelta, double deltaUp, double deltaDown, int longShort)
	{
		this->deltaUp = deltaUp; this->deltaDown = deltaDown; originalUnitSize = 40;
		this->originalDelta = originalDelta; this->longShort = longShort;
		localLiquidityIndicator = liquidityIndicator(originalDelta, deltaUp, deltaDown, originalDelta*2.525729, 50.0);
		initialized = false; realizedProfit = 0.0; positionRealizedProfit = 0.0;  inventory = 0; unrealizedProfit = 0; priceMove = 0;
		dcOros = 0; ind = 0; buyCorrectedInFunction = false; sellCorrectedInFunction = false; zero = 0;
		cascadeVol = 40; orderPrice = 0.0; marketPrice = 0.0; targetAbsPnL = 0.0; events = 0;
		buyOrderVolume = 0.0; sellOrderVolume = 0.0; mode = 0;
		buyLimitOrder = LimitOrder(0, Price(0.0, 0.0), 0.0, 0, 0.0, 0, 0); sellLimitOrder = LimitOrder(0, Price(0.0, 0.0), 0.0, 0, 0.0, 0, 0);
		disBalancedOrders.clear(); tempObj = LimitOrder(0, Price(0.0, 0.0), 0.0, 0, 0.0, 0, 0); tempList.clear();
	}

	bool run(Price price)
	{
		localLiquidityIndicator.liquidityEstimation(price);
		mode = localLiquidityIndicator.getEvents();
		toReturn = false;
		if (!initialized && mode!=0)
		{
			inventory = 0;
			initialized = true; 
			dcOros = mode;
			putOrders(price);
		}
		else
		{
			if (checkBuyFilled(price))
			{
				toReturn = true;
				return toReturn;
			}
			else if (checkSellFilled(price))
			{
				toReturn = true;
				return toReturn;
			}
			else
			{
				correctOrdersLevel(price);
				toReturn = true; 
			}
			if (positionCrossedTargetPnL(price))
			{
				buyLimitOrder.changeVolume(0);
				sellLimitOrder.changeVolume(0);
				closePosition(price);
			}
		}
		return toReturn;
	}

	bool putOrders(Price price)
	{
		computeUnitSizeLiq(localLiquidityIndicator.liquidity);
		correctThresholdsAndVolumes(inventory);
		if (longShort == 1)
		{
			switch (dcOros)
			{
			default:
				return false;
			case -1:
				if (disBalancedOrders.empty())
				{
					buyLimitOrder = LimitOrder(1, price.clone(), price.getbid()*(1 - deltaDown), cascadeVol, deltaDown, dcOros, -2);
					sellLimitOrder = LimitOrder(-1, price.clone(), price.getask()*(1 + deltaUp), 0, deltaUp, dcOros, 1);
					computeTargetAbsPnL(buyLimitOrder);
				}
				else
				{
					buyLimitOrder = LimitOrder(1, price.clone(), price.getbid()*(1 - deltaDown), cascadeVol, deltaDown, dcOros, -2);
					sellLimitOrder = LimitOrder(-1, price.clone(), price.getask()*(1 + deltaUp), disBalancedOrders.front().getVolume(), deltaUp, dcOros, 1);
				}
				return true;
			case 1:
				if (disBalancedOrders.empty())
				{
					buyLimitOrder = LimitOrder(1, price.clone(), price.getbid()*(1 - deltaDown), cascadeVol, deltaDown, dcOros, -1);
					sellLimitOrder = LimitOrder(-1, price.clone(), price.getask()*(1 + deltaUp), 0, deltaUp, dcOros, 2);
					computeTargetAbsPnL(buyLimitOrder);
				}
				else
				{
					buyLimitOrder = LimitOrder(1, price.clone(), price.getbid()*(1 - deltaDown), cascadeVol, deltaDown, dcOros, -1);
					sellLimitOrder = LimitOrder(-1, price.clone(), price.getask()*(1 + deltaUp), disBalancedOrders.front().getVolume(), deltaUp, dcOros, 2);
				}
				return true;
			case -2:
				if (disBalancedOrders.empty())
				{
					buyLimitOrder = LimitOrder(1, price.clone(), price.getbid()*(1 - deltaDown), cascadeVol, deltaDown, dcOros, -2);
					sellLimitOrder = LimitOrder(-1, price.clone(), price.getask()*(1 + deltaUp), 0, deltaUp, dcOros, 1);
					computeTargetAbsPnL(buyLimitOrder);
				}
				else
				{
					buyLimitOrder = LimitOrder(1, price.clone(), price.getbid()*(1 - deltaDown), cascadeVol, deltaDown, dcOros, -2);
					sellLimitOrder = LimitOrder(-1, price.clone(), price.getask()*(1 + deltaUp), disBalancedOrders.front().getVolume(), deltaUp, dcOros, 1);
				}
				return true;
			case 2:
				if (disBalancedOrders.empty())
				{
					buyLimitOrder = LimitOrder(1, price.clone(), price.getbid()*(1 - deltaDown), cascadeVol, deltaDown, dcOros, -1);
					sellLimitOrder = LimitOrder(-1, price.clone(), price.getask()*(1 + deltaUp), 0, deltaUp, dcOros, 2);
					computeTargetAbsPnL(buyLimitOrder);
				}
				else
				{
					buyLimitOrder = LimitOrder(1, price.clone(), price.getbid()*(1 - deltaDown), cascadeVol, deltaDown, dcOros, -1);
					sellLimitOrder = LimitOrder(-1, price.clone(), price.getask()*(1 + deltaUp), disBalancedOrders.front().getVolume(), deltaUp, dcOros, 2);
				}
				return true;
			}
		}
		else
		{
			switch (dcOros)
			{
			default:
				return false;
			case -1:
				if (disBalancedOrders.empty())
				{
					buyLimitOrder = LimitOrder(1, price.clone(), price.getbid()*(1 - deltaDown), 0, deltaDown, dcOros, -2);
					sellLimitOrder = LimitOrder(-1, price.clone(), price.getask()*(1 + deltaUp), cascadeVol, deltaUp, dcOros, 1);
					computeTargetAbsPnL(sellLimitOrder);
				}
				else
				{
					buyLimitOrder = LimitOrder(1, price.clone(), price.getbid()*(1 - deltaDown), disBalancedOrders.front().getVolume(), deltaDown, dcOros, -2);
					sellLimitOrder = LimitOrder(-1, price.clone(), price.getask()*(1 + deltaUp), cascadeVol, deltaUp, dcOros, 1);
				}
				return true;
			case 1:
				if (disBalancedOrders.empty())
				{
					buyLimitOrder = LimitOrder(1, price.clone(), price.getbid()*(1 - deltaDown), 0, deltaDown, dcOros, -1);
					sellLimitOrder = LimitOrder(-1, price.clone(), price.getask()*(1 + deltaUp), cascadeVol, deltaUp, dcOros, 2);
					computeTargetAbsPnL(sellLimitOrder);
				}
				else
				{
					buyLimitOrder = LimitOrder(1, price.clone(), price.getbid()*(1 - deltaDown), disBalancedOrders.front().getVolume(), deltaDown, dcOros, -1);
					sellLimitOrder = LimitOrder(-1, price.clone(), price.getask()*(1 + deltaUp), cascadeVol, deltaUp, dcOros, 2);
				}
				return true;
			case -2:
				if (disBalancedOrders.empty())
				{
					buyLimitOrder = LimitOrder(1, price.clone(), price.getbid()*(1 - deltaDown), 0, deltaDown, dcOros, -2);
					sellLimitOrder = LimitOrder(-1, price.clone(), price.getask()*(1 + deltaUp), cascadeVol, deltaUp, dcOros, 1);
					computeTargetAbsPnL(sellLimitOrder);
				}
				else
				{
					buyLimitOrder = LimitOrder(1, price.clone(), price.getbid()*(1 - deltaDown), disBalancedOrders.front().getVolume(), deltaDown, dcOros, -2);
					sellLimitOrder = LimitOrder(-1, price.clone(), price.getask()*(1 + deltaUp), cascadeVol, deltaUp, dcOros, 1);
				}
				return true;
			case 2:
				if (disBalancedOrders.empty())
				{
					buyLimitOrder = LimitOrder(1, price.clone(), price.getbid()*(1 - deltaDown), 0, deltaDown, dcOros, -1);
					sellLimitOrder = LimitOrder(-1, price.clone(), price.getask()*(1 + deltaUp), cascadeVol, deltaUp, dcOros, 2);
					computeTargetAbsPnL(sellLimitOrder);
				}
				else
				{
					buyLimitOrder = LimitOrder(1, price.clone(), price.getbid()*(1 - deltaDown), disBalancedOrders.front().getVolume(), deltaDown, dcOros, -1);
					sellLimitOrder = LimitOrder(-1, price.clone(), price.getask()*(1 + deltaUp), cascadeVol, deltaUp, dcOros, 2);
				}
				return true;
			}
		}
	}

	bool checkBuyFilled(Price price)
	{
		buyFilled = false;
		if (buyLimitOrder.getVolume() != 0)
		{
			if (price.getbid() < buyLimitOrder.getLevel())
			{
				inventory += buyLimitOrder.getVolume();
				correctThresholdsAndVolumes(inventory);
				if (longShort == 1)
				{
					disBalancedOrders.push_front(buyLimitOrder.clone());
				}
				else
				{
					double priceMove = (buyLimitOrder.getLevel() - disBalancedOrders.front().getLevel()) * longShort;
					positionRealizedProfit += priceMove * buyLimitOrder.getVolume();
					disBalancedOrders.pop_front();
				}

				if (disBalancedOrders.empty())
				{
					closePosition(price);
				}
				dcOros = buyLimitOrder.getDcRos();
				putOrders(price);
				buyFilled = true;
				return buyFilled;
			}
			return buyFilled;
		}
		else
		{
			if (price.getbid() < buyLimitOrder.getLevel())
			{
				dcOros = buyLimitOrder.getDcRos(); 
				putOrders(price);
				buyFilled = true;
				return buyFilled;
			}
			return buyFilled;
		}
	}

	bool checkSellFilled(Price price)
	{
		sellFilled = false;
		if (sellLimitOrder.getVolume() != 0)
		{

			if (price.getask() > sellLimitOrder.getLevel())
			{
				inventory -= sellLimitOrder.getVolume();
				correctThresholdsAndVolumes(inventory);
				if (longShort == -1)
				{
					disBalancedOrders.push_front(sellLimitOrder.clone());
				}
				else
				{
					double priceMove = (sellLimitOrder.getLevel() - disBalancedOrders.front().getLevel()) * longShort;
					positionRealizedProfit += priceMove * sellLimitOrder.getVolume();
					disBalancedOrders.pop_front();
				}

				if (disBalancedOrders.empty())
				{
					closePosition(price);
				}
				dcOros = sellLimitOrder.getDcRos();
				putOrders(price);
				sellFilled = true;
				return sellFilled;
			}
			return sellFilled;
		}
		else
		{
			if (price.getask() > sellLimitOrder.getLevel())
			{
				dcOros = sellLimitOrder.getDcRos();
				putOrders(price);
				sellFilled = true;
				return sellFilled;
			}
			return sellFilled;
		}
	}

	bool correctOrdersLevel(Price price)
	{
		if (correctBuyLimitOrder(price) == true || correctSellLimitOrder(price) == true) { return true; }
		else { return false; }
	}


	bool correctBuyLimitOrder(Price price)
	{
		buyCorrectedInFunction = false;
		if ((buyLimitOrder.getDcRos() == 1) || (buyLimitOrder.getDcRos() == -1))
		{
			if ((-log(buyLimitOrder.getLevel() / price.getbid())) > buyLimitOrder.getDelta())
			{
				buyLimitOrder.setLevel(price.getbid() * (1.0 - buyLimitOrder.getDelta()));
				buyCorrectedInFunction = true;
				return buyCorrectedInFunction;
			}
		}
		return buyCorrectedInFunction;
	}


	bool correctSellLimitOrder(Price price)
	{
		sellCorrectedInFunction = false;
		if ((sellLimitOrder.getDcRos() == 1) || (sellLimitOrder.getDcRos() == -1))
		{
			if ((log(sellLimitOrder.getLevel() / price.getask())) > sellLimitOrder.getDelta())
			{
				sellLimitOrder.setLevel(price.getask() * (1.0 + sellLimitOrder.getDelta()));
				sellCorrectedInFunction = true;
				return sellCorrectedInFunction;
			}
		}
		return sellCorrectedInFunction;
	}

	double computeUpDownRatio(int inventory)
	{
		if (longShort == 1)
		{
			if (inventory >= 3000.0)
			{
				return 0.25;
			}
			else if (inventory >= 1500.0 && inventory < 3000.0)
			{
				return 0.5;
			}
			else
				return 1.0;
		}
		else
		{
			if (inventory >= -3000.0 && inventory < -1500.0)
			{
				return 2.0;
			}
			else if (inventory <= -3000.0)
			{
				return 4.0;
			}
			else
			{
				return 1.0;
			}
		}
	}

	void correctThresholdsAndVolumes(int inventory)
	{
		upDownRatio = computeUpDownRatio(inventory);
		if (upDownRatio == 4.0)
		{
			deltaUp = originalDelta * 2.0;
			deltaDown = originalDelta * 0.5;
			cascadeVol = unitSizeLiq / 4.0;
		}
		else if (upDownRatio == 2.0)
		{
			deltaUp = originalDelta * 1.5;
			deltaDown = originalDelta * 0.75;
			cascadeVol = unitSizeLiq / 2.0;
		}
		else if (upDownRatio == 0.5)
		{
			deltaUp = originalDelta * 0.75;
			deltaDown = originalDelta * 1.5;
			cascadeVol = unitSizeLiq / 2.0;
		}
		else if (upDownRatio == 0.25)
		{
			deltaUp = originalDelta * 0.5;
			deltaDown = originalDelta * 2.0;
			cascadeVol = unitSizeLiq / 4.0;
		}
		else
		{
			deltaUp = originalDelta * 1.0;
			deltaDown = originalDelta * 1.0;
			cascadeVol = unitSizeLiq;
		}
	}

	int computeUnitSizeLiq(double liquidity)
	{
		if (0.5 <= liquidity)
		{
			unitSizeLiq = 10 * originalUnitSize;
		}
		else if (0.1 <= liquidity && liquidity < 0.5)
		{
			unitSizeLiq = 5 * originalUnitSize;
		}
		else
		{
			unitSizeLiq = 1 * originalUnitSize;
		}
		return unitSizeLiq;
	}

	bool positionCrossedTargetPnL(Price price)
	{
		if (getPositionTotalPnL(price) >= targetAbsPnL) { return true; }
		else { return false; }
	}

	double getPositionTotalPnL(Price price)
	{
		return getPositionProfit(price);
	}

	double getPositionProfit(Price price)
	{
		return positionRealizedProfit + getPositionUnrealizedProfit(price);
	}

	double getPositionUnrealizedProfit(Price price)
	{
		if (!disBalancedOrders.empty())
		{
			marketPrice = (longShort == 1 ? price.getbid() : price.getask());
			unrealizedProfit = 0.0;
			tempList.clear(); 
			tempList = disBalancedOrders;
			for (auto i = disBalancedOrders.begin(); i != disBalancedOrders.end(); i++)
			{
				tempObj = tempList.front(); tempList.pop_front();
				unrealizedProfit += ((marketPrice - tempObj.getLevel()) * tempObj.getType()) * tempObj.getVolume();
			}
			return unrealizedProfit;
		}
		return 0.0;
	}

	void closePosition(Price price)
	{
		marketOrderToClosePosition(price);
		realizedProfit += positionRealizedProfit;
		positionRealizedProfit = 0.0;
		inventory = 0;
		correctThresholdsAndVolumes(inventory);
	}

	bool marketOrderToClosePosition(Price price)
	{
		tempList.clear();
		tempList = disBalancedOrders;
		orderPrice = (longShort == 1 ? price.getbid() : price.getask());
		for (auto i = disBalancedOrders.begin(); i != disBalancedOrders.end(); i++)
		{
			tempObj = tempList.front(); 
			priceMove = (orderPrice - tempObj.getLevel()) * tempObj.getType();
			positionRealizedProfit += priceMove * tempObj.getVolume();
			tempList.pop_front();
		}
		disBalancedOrders.clear();
		return true;
	}

	void computeTargetAbsPnL(LimitOrder firstCascadingOrder)
	{
		targetAbsPnL = firstCascadingOrder.getVolume() * firstCascadingOrder.getDelta() * firstCascadingOrder.getLevel();
	}

	double getRealizedProfit() { return realizedProfit; }

	int getInventory() { return inventory; }

private:
	double originalDelta, deltaUp, deltaDown, realizedProfit, targetAbsPnL,
		positionRealizedProfit, orderPrice, marketPrice, unrealizedProfit;
	int mode, longShort, dcOros, inventory, originalUnitSize, unitSizeLiq, cascadeVol, decascadingVol;
	bool initialized, toReturn, buyCorrected, sellCorrected, buyCorrectedInFunction, sellCorrectedInFunction, buyFilled, sellFilled;
	double priceMove, upDownRatio;
	int zero, buyOrderVolume, sellOrderVolume, ind, events;
	LimitOrder buyLimitOrder;
	LimitOrder sellLimitOrder;
	LimitOrder tempObj;
	forward_list <LimitOrder> disBalancedOrders, tempList;
	liquidityIndicator localLiquidityIndicator;
};