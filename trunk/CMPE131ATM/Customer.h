#pragma once
#include "Account.h"

class Customer
{
public: // constructors
	Customer(void);
	virtual ~Customer(void);

public: // instance methods
	Account* getAccount(void);
	void setAccount(Account* account);

	int getCustomerID(void);
	void setCustomerID(int customerID);

	int getIsAuthorized();
	void setIsAuthorized(int isAuthorized);

protected: // member variables
	int customer_id;
	int is_authorized;

	Account* account; // TODO: more than one account
};

