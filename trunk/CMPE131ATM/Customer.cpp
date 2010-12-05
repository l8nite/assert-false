#include "StdAfx.h"
#include "Customer.h"

Customer::Customer(void) {
	this->customer_id = 0;
	this->is_authorized = 0;
	this->accounts.clear();
}

Customer::~Customer(void) {
	for (int i = 0; i < this->accounts.size(); ++i) {
		delete this->accounts[i];
	}
	
	this->accounts.clear();
}

vector<Account*> Customer::getAccounts(void) {
	return this->accounts;
}

void Customer::addAccount(Account* account) {
	this->accounts.push_back(account);
}

int Customer::getCustomerID(void) {
	return this->customer_id;
}

void Customer::setCustomerID(int customerID) {
	this->customer_id = customerID;
}

int Customer::getIsAuthorized(void) {
	return this->is_authorized;
}

void Customer::setIsAuthorized(int isAuthorized) {
	this->is_authorized = isAuthorized;
}