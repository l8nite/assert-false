#pragma once
#include <string>
#include <iostream>

using namespace std;

class Account
{
public:
	Account(void);

public:
	int getAccountID(void);
	void setAccountID(int accountID);

	int getBalance(void);
	void setBalance(int balance);

	string getLabel(void);
	void setLabel(string label);

	// prints information about the account
	void printSummary();

protected:
	int account_id;
	int balance;
	string label;
};