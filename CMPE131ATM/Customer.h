#pragma once
#include "Account.h"
#include <vector>

class Customer
{
public: // constructors
	Customer(void);
	virtual ~Customer(void);

public: // instance methods
	vector<Account*> getAccounts(void);
	void addAccount(Account* account);

	int getCustomerID(void);
	void setCustomerID(int customerID);

	int getIsAuthorized();
	void setIsAuthorized(int isAuthorized);

protected: // member variables
	int customer_id;
	int is_authorized;
	vector<Account*> accounts;
};

