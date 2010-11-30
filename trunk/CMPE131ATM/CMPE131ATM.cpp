// CMPE131ATM.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include "Customer.h"
#include "sqlite3.h"

using namespace std;

// application functions
Customer* loginCustomer(sqlite3* dbh);
static int _db_load_customer(void* customer, int argc, char** argv, char** azColName);

void applicationMenuLoop(Customer* customer, sqlite3* dbh);
void handleDeposit(Customer* customer, sqlite3* dbh);
void handleWithdrawal(Customer* customer, sqlite3* dbh);

// the database filename we'll connect to (using sqlite3)
const char* gDSN = "cmpe131atm.sqlite";

// how many login attempts we allow before exiting
static const int gLoginAttemptsAllowed = 3;

// lets us convert string input to native types we desire
template <class T>
bool from_string(T& t, 
                 const std::string& s, 
                 std::ios_base& (*f)(std::ios_base&))
{
  std::istringstream iss(s);
  return !(iss >> f >> t).fail();
}


// entry point for our application
void main(int argc, _TCHAR* argv[])
{
	// connect to the database
	sqlite3* dbh = NULL;
	int rc = sqlite3_open(gDSN, &dbh);

	if (rc == SQLITE_OK) {
		fprintf(stderr, "Successfully connected to database.\n");
	}
	else {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(dbh));
		goto exit;
	}

	// log in a customer
	Customer* customer = loginCustomer(dbh);

	if (NULL == customer) { // we couldn't log them in
		rc = -1;
		goto exit;
	}

	applicationMenuLoop(customer, dbh); // main loop

	exit:
	if (NULL != customer) { // allocated by loginCustomer
		delete customer;
	}

	if (NULL != dbh) {
		sqlite3_close(dbh);	// disconnect from the database
		dbh = NULL;
	}

	// exit with error code (if set)
	exit(rc);
}


// prompts the customer to input their credentials and returns a newly instantiated
// Customer object if they are successful at logging in.
Customer* loginCustomer(sqlite3* dbh) {
	assert(NULL != dbh); // database handle should be initialized 
	int login_attempts = 0;

get_customer_number:
	login_attempts++;

	int customer_id = 0;
	do {
		cout << "Please input your customer number: ";
		string buffer;
		getline(cin, buffer);

		if (! from_string<int>(customer_id, buffer, std::dec)) {
			customer_id = 0;
		}
	} while(!customer_id);

//get_security_code:
	int customer_pin = 0;
	do {
		cout << "Please input your security code: ";
		string buffer;
		getline(cin, buffer);

		if (! from_string<int>(customer_pin, buffer, std::dec)) {
			customer_pin = 0;
		}
	} while(!customer_pin);

	// validate against database
	Customer* customer = new Customer();

	stringstream sql;
	sql << "SELECT * FROM accounts WHERE customer_id=" << customer_id
		<< " AND customer_pin=" << customer_pin;
	char* sqlite3_error;
	int rc = sqlite3_exec(dbh, sql.str().c_str(),
		_db_load_customer, customer, &sqlite3_error);

	if (SQLITE_OK != rc) {
		cerr << "Error reading database: " << sqlite3_errmsg(dbh) << endl;
		sqlite3_free(sqlite3_error);
		delete customer;
		return NULL;
	}

	if (!customer->getIsAuthorized()) {
		if(login_attempts == gLoginAttemptsAllowed) {
			cout << "Too many attempts.  Good bye." << endl;
			delete customer;
			return NULL;
		}

		cout << "Invalid login credentials, please try again..." << endl;

		goto get_customer_number;
	}

	customer->getAccount()->printSummary();

	return customer;
}


// callback for database login / loading routine 
static int _db_load_customer(void* customer, int argc, char** argv, char** azColName) {
	Account* account = new Account();
	// id, customer_id, customer_pin, balance
	account->setCustomerID(atol(argv[1]));
	account->setBalance(atof(argv[3]));
	((Customer*)customer)->setAccount(account);
	((Customer*)customer)->setIsAuthorized(1);
	return 0;
}


// main application loop
void applicationMenuLoop(Customer* customer, sqlite3* dbh) {
	menu:
	cout << "1. Print Account Summary" << endl;
	cout << "2. View Transaction History" << endl;
	cout << "3. Deposit Funds" << endl;
	cout << "4. Withdraw Funds" << endl;
	cout << "5. Exit" << endl;

	int menu_choice = 0;
	do {
		cout << "Choice? ";
		cin >> menu_choice;
	}
	while (menu_choice < 1 || menu_choice > 5); // TODO: error on invalid?

	switch (menu_choice) { // options from the menu above
	case 1:
		customer->getAccount()->printSummary();
		break;
	case 2:
		customer->getAccount()->printHistory();
		break;
	case 3:
		handleDeposit(customer, dbh);	
		break;
	case 4:
		handleWithdrawal(customer, dbh);
		break;
	case 5:
		return;
		break;
	}

	goto menu;
}


void handleDeposit(Customer* customer, sqlite3* dbh) {
	double depositAmount = 0.0;
	
	do {
    	cout << "Enter deposit amount: $";
    	cin >> depositAmount;
	}
	while (!depositAmount); // TODO validate amount (positive)
	
	// TODO: protect against overflowing double

	double balance = customer->getAccount()->depositFunds(depositAmount);
	printf("Your new balance is: $%.2f\n", balance);
}


void handleWithdrawal(Customer* customer, sqlite3* dbh) {
	double withdrawalAmount = 0.0;
	
	do {
    	cout << "Enter withdrawal amount: $";
    	cin >> withdrawalAmount;
	}
	while (!withdrawalAmount); // TODO validate amount (positive)

	// TODO: protect against overdrafting of the account

	double balance = customer->getAccount()->withdrawFunds(withdrawalAmount);
	printf("Your new balance is: $%.2f\n", balance);
}