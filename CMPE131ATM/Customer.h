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

	//gets customerid value
	//@param none
	//@retun customerid 
	int getCustomerID(void);

	//sets customerid value
	//@param customerid
	//@return none
	void setCustomerID(int customerID);

	//gets isAuthorized value
	//@param none
	//@retun isAuthorized 
	int getIsAuthorized();

	//sets isAuthorized value
	//@param isAuthorized
	//@return none
	void setIsAuthorized(int isAuthorized);

protected: // member variables
	int customer_id;   //customer_id: id of customer
	int is_authorized; //is_authorized: value of authorization during login

	Account* account; // TODO: more than one account
};

