#include "StdAfx.h"
#include "Customer.h"

Customer::Customer(void) {
	this->account = NULL;
}

Customer::Customer(int accountID) {
	this->account = new Account(accountID);
}

Customer::~Customer(void) {
	if (this->account) {
		delete this->account;
	}
}

Account* Customer::getAccount(void) {
	return this->account;
}

void Customer::setAccount(Account* account) {
	if (this->account) {
		delete this->account;
	}

	this->account = account;
}