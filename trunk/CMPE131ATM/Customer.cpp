#include "StdAfx.h"
#include "Customer.h"

Customer::Customer(void) {
	this->account = NULL;
	this->customer_id = 0;
	this->is_authorized = 0;
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