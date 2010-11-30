#include "StdAfx.h"
#include "Account.h"
#include <iostream>
#include "sqlite3.h"

static int account_select_thunk(void* obj, int argc, char** argv, char** azColName) {
	((Account*)obj)->populate_from_database(argc, argv, azColName);
	return 0;
}

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

void Account::populate_from_database(int argc, char** argv, char** azColName) {
	this->balance = strtod(argv[2],NULL);
}

void Account::setCustomerID(int customerID) {
	this->customer_id = customerID;
	printf("Loading customer ID: %i\n", customerID);

	sqlite3 *db;
	char sql[512];
	sprintf_s(sql, "SELECT * FROM accounts WHERE customer=%d", customerID);
	char* err;
	int rc;

	rc = sqlite3_open("cmpe131atm.sqlite", &db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return;
	}
	else {
		fprintf(stderr, "Opened database successfully\n");
	}

	rc = sqlite3_exec(db, sql, account_select_thunk, this, &err);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", err);
		sqlite3_free(err);
		return;
	}

	sqlite3_close(db);
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