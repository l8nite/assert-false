#include "StdAfx.h"
#include "Account.h"

using namespace std;

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
}

string Account::getLabel(void) { 
	return this->label;
}

void Account::setLabel(string label) {
	this->label = label;
}

void Account::printSummary(void) {
	cout << "Account #: " << this->account_id
		 << " ('" << this->label << "') --"
		 << " Current Balance: ";
	printf("$%.2f\n", (float)this->balance / 100.0f);
}