#pragma once
#include<iostream>
#include "LimitOrder.h"

using namespace std;
struct node
{
	LimitOrder data;
	node *next;
};
class list
{
private:
	node *head, *tail;
public:
	list()
	{
		head = NULL;
		tail = NULL;
	}
	void createnode(LimitOrder value)
	{
		node *temp = new node;
		temp->data = value;
		temp->next = NULL;
		if (head == NULL)
		{
			head = temp;
			tail = temp;
			temp = NULL;
		}
		else
		{
			tail->next = temp;
			tail = temp;
		}
	}
	void display()
	{
		node *temp = new node;
		temp = head;
		while (temp != NULL)
		{
			//cout << temp->data << "\t";
			temp = temp->next;
		}
	}
	void insert_start(LimitOrder value)
	{
		node *temp = new node;
		temp->data = value;
		temp->next = head;
		head = temp;
	}
	void insert_position(int pos, LimitOrder value)
	{
		node *pre = new node;
		node *cur = new node;
		node *temp = new node;
		cur = head;
		for (int i = 1; i<pos; i++)
		{
			pre = cur;
			cur = cur->next;
		}
		temp->data = value;
		pre->next = temp;
		temp->next = cur;
	}
	void delete_first()
	{
		node *temp = new node;
		temp = head;
		head = head->next;
		delete temp;
	}
	void delete_last()
	{
		node *current = new node;
		node *previous = new node;
		current = head;
		while (current->next != NULL)
		{
			previous = current;
			current = current->next;
		}
		tail = previous;
		previous->next = NULL;
		delete current;
	}
	void delete_position(int pos)
	{
		node *current = new node;
		node *previous = new node;
		current = head;
		for (int i = 1; i<pos; i++)
		{
			previous = current;
			current = current->next;
		}
		previous->next = current->next;
	}

	LimitOrder get_last()
	{
		node *current = new node;
		node *previous = new node;
		current = head;
		while (current->next != NULL)
		{
			previous = current;
			current = current->next;
		}
		tail = previous;
		previous->next = NULL;
		return current->data;
	}

	LimitOrder get_position(int pos)
	{
		node *current = new node;
		node *previous = new node;
		LimitOrder requiredLimitOrder;
		current = head;
		if (pos == 1)
		{
			return requiredLimitOrder = head->data;
		}
		else 
		{
			for (int i = 1; i<pos; i++)
			{
				previous = current;
				current = current->next;
			}
			requiredLimitOrder = current->data;
		}
		return requiredLimitOrder;
	}

	int get_size()
	{
		node *temp = new node;
		temp = head;
		int i = 0;
		while (temp != NULL)
		{
			temp = temp->next;
			i++;
		}
		return i;
	}
};