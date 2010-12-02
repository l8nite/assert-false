#include "StdAfx.h"
#include "Account.h"
#include <iostream>

Account::Account(void) {
	this->balance = 0;
	this->account_id = 0;
}

int Account::getAccountID(void) {
	return this->account_id;
}

void Account::setAccountID(int accountID) {
	this->account_id = accountID;
}

int Account::getBalance(void) {
	return this->balance;
}

void Account::setBalance(int balance) {
	this->balance = balance;
	// TODO: log balance update in transaction history
}

int Account::withdrawFunds(int withdrawalAmount) {
	this->setBalance(this->getBalance() - withdrawalAmount);
	return this->balance;
}

int Account::depositFunds(int depositAmount) {
	this->setBalance(this->getBalance() + depositAmount);
	return this->balance;
}

void Account::printSummary(void) {
	printf("Account #: %d -- Current Balance: $%.2f\n", this->account_id, (this->balance / 100.0f));
}

void Account::printHistory(void) {
	std::cout << "Account history goes here" << std::endl;
}