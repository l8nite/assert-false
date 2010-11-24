#include "StdAfx.h"
#include "Account.h"
#include <iostream>

Account::Account(void) {
	this->customer_id = 0;
	this->balance = 0.0;
}

Account::Account(int customerID) {
	this->setCustomerID(customerID);
}

int Account::getCustomerID(void) {
	return this->customer_id;
}

void Account::setCustomerID(int customerID) {
	this->customer_id = customerID;
	// TODO: really load the customer
	printf("Loading customer ID: %i\n", customerID);
	this->setBalance(123.45);
}

double Account::getBalance (void) {
	return this->balance;
}

void Account::setBalance(double balance) {
	this->balance = balance;
	// TODO: log balance update in transaction history
	printf("Balance updated to: $%.2f\n", balance);
}

double Account::withdrawFunds(double withdrawalAmount) {
	this->setBalance(this->getBalance() - withdrawalAmount);
	return this->balance;
}

double Account::depositFunds(double depositAmount) {
	this->setBalance(this->getBalance() + depositAmount);
	return this->balance;
}

void Account::printSummary(void) {
	// TODO: should accounts have an ID separate from the customer ID? 
	//       probably not necessary for release 1 since we only allow one account per customer anyway
	printf("Account #: %d -- Current Balance: $%.2f\n", this->customer_id, this->balance);
}

void Account::printHistory(void) {
	// TODO: load transaction history
	std::cout << "Account history goes here" << std::endl;
}