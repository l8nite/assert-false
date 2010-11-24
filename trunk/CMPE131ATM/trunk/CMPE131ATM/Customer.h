#pragma once
#include "Account.h"

class Customer
{
public: // constructors
	Customer(void);
	Customer(int accountID);
	virtual ~Customer(void);

public: // instance methods
	Account* getAccount();
	void setAccount(Account* account);

protected: // member variables
	Account* account;
};

