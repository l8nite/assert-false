// CMPE131ATM.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "Customer.h"
#include "sqlite3.h"

using namespace std;

/*
 * application logical flow functions
 */
// handles authenticating the customer number and PIN
Customer* loginCustomer(sqlite3* dbh);

// main menu
void applicationMenuLoop(Customer* customer, sqlite3* dbh);

// handles deposit/withdrawal user interaction
void handleDeposit(Account* account, sqlite3* dbh);
void handleWithdrawal(Account* account, sqlite3* dbh);

// handles printing out an account's transaction history
void handleTransactionHistory(Account* account, sqlite3* dbh);


/*
 * utility functions
 */
// callback used by sqlite3_exec to populate the given Customer instance
static int _db_load_customer(void* customer, 
	int argc, char** argv, char** cols);

// callback used by sqlite3_exec to print out a Customer's transactions
static int _db_view_transaction(void* unused,
	int argc, char** argv, char** cols);

// updates the database for an account balance change
void _update_account_balance(Account* account, int balance, sqlite3* dbh);

// adds a new transaction to the transactions table
void _update_transaction_log(Account* account, char* txType, int amount, 
	sqlite3* dbh);

static int _repeatedly_prompt_user_for_positive_numeric_input
	(const char* prompt, const char* error, int isMoney);

// lets us convert string input to native types we desire
template <class T>
bool from_string(T& t, const std::string& s, 
	std::ios_base& (*f)(std::ios_base&))
{
	std::istringstream iss(s);
	return !(iss >> f >> t).fail();
}



/*
 * globals
 */
// the database filename we'll connect to (using sqlite3)
const char* gDSN = "cmpe131atm.sqlite";

// how many login attempts we allow before exiting
static const int gLoginAttemptsAllowed = 3;


/*
 * entry point
 */
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

	Customer* customer = loginCustomer(dbh);

	if (NULL == customer) { // we couldn't log them in
		rc = -1;
		goto exit;
	}

	// enter the main application loop
	applicationMenuLoop(customer, dbh);

	// clean up after ourselves and then exit
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


// prompts the customer to input their credentials and returns a newly created
// Customer object if they are successful at logging in.
Customer* loginCustomer(sqlite3* dbh) {
	assert(NULL != dbh); // database handle should be initialized 
	int login_attempts = 0;

get_customer_number: // execution flow returns here if the user fails to auth
	login_attempts++;

	int customer_id = _repeatedly_prompt_user_for_positive_numeric_input(
		"Please input your customer number: ", NULL, 0);

	int customer_pin = _repeatedly_prompt_user_for_positive_numeric_input(
		"Please input your security code: ", NULL, 0);

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
static int _db_load_customer(void* customer, 
	int argc, char** argv, char** cols)
{
	Account* account = new Account();

	// load database row into the account/Customer object
	// argv = { id, customer_id, customer_pin, balance }
	account->setAccountID(atoi(argv[0]));
	((Customer*)customer)->setCustomerID(atol(argv[1]));
	account->setBalance(atoi(argv[3]));

	// add account to the customer object, authorize them
	((Customer*)customer)->setAccount(account);
	((Customer*)customer)->setIsAuthorized(1);

	return 0;
}


// main menu loop
void applicationMenuLoop(Customer* customer, sqlite3* dbh) {
	menu:
	cout << "1. Print Account Summary" << endl;
	cout << "2. View Transaction History" << endl;
	cout << "3. Deposit Funds" << endl;
	cout << "4. Withdraw Funds" << endl;
	cout << "5. Exit" << endl;

	int menu_choice = _repeatedly_prompt_user_for_positive_numeric_input(
		"Choice? ", "Invalid choice.  Try again.", 0);

	if (menu_choice < 1 || menu_choice > 5) {
		cout << "Invalid choice.  Try again." << endl;
		goto menu;
	}

	switch (menu_choice) { // options from the menu above
	case 1:
		customer->getAccount()->printSummary();
		break;
	case 2:
		handleTransactionHistory(customer->getAccount(), dbh);
		break;
	case 3:
		handleDeposit(customer->getAccount(), dbh);	
		break;
	case 4:
		handleWithdrawal(customer->getAccount(), dbh);
		break;
	case 5:
		return;
		break;
	}

	goto menu;
}


// handle gathering user input for a deposit and updating the database
void handleDeposit(Account* account, sqlite3* dbh) {
	int deposit_amount = _repeatedly_prompt_user_for_positive_numeric_input(
		"Enter deposit amount: $", "Invalid deposit.  Try again.", 1);

	// bail out if they ended up deciding not to deposit anything.
	if (!deposit_amount) {
		return;
	}

	int current_balance = account->getBalance();
	_update_account_balance(account, current_balance + deposit_amount, dbh);
	_update_transaction_log(account, "Deposit", deposit_amount, dbh);
}


// handle gathering user input for a withdrawal and updating the database
void handleWithdrawal(Account* account, sqlite3* dbh) {
	int current_balance = account->getBalance();

	enter_withdrawal_amount:
	int withdrawal_amount = _repeatedly_prompt_user_for_positive_numeric_input(
		"Enter withdrawal amount: $", "Invalid withdrawal.  Try again.", 1);

	// bail out if they ended up deciding not to withdraw anything
	if (!withdrawal_amount) {
		return;
	}

	// check for sufficient funds
	if (withdrawal_amount > current_balance) {
		cout << "You do not have sufficient funds.  Try again." << endl;
		goto enter_withdrawal_amount;
	}

	_update_account_balance(account, current_balance - withdrawal_amount, dbh);
	_update_transaction_log(account, "Withdrawal", withdrawal_amount, dbh);
}


// handles the database interaction for updating an account balance
void _update_account_balance(Account* account, int balance, sqlite3* dbh) {
	stringstream sql;
	sql << "UPDATE accounts SET balance=" << balance
		<< " WHERE id=" << account->getAccountID();
	char* sqlite3_error;
	int rc = sqlite3_exec(dbh, sql.str().c_str(), NULL, NULL, &sqlite3_error);

	if (SQLITE_OK != rc) {
		cerr << "Error updating database: " << sqlite3_errmsg(dbh) << endl;
		sqlite3_free(sqlite3_error);
		return;
	}

	account->setBalance(balance);

	printf("Your new balance is: $%.2f\n", (float)balance / 100.0f);
}


// handles the database interaction for viewing a customer's transaction log
void handleTransactionHistory(Account* account, sqlite3* dbh) {
	stringstream sql;
	sql << "SELECT * FROM transactions WHERE account_id=" 
		<< account->getAccountID()
		<< " ORDER BY timestamp DESC";

	char* sqlite3_error;
	int rc = sqlite3_exec(dbh, sql.str().c_str(),
		_db_view_transaction, NULL, &sqlite3_error);

	if (SQLITE_OK != rc) {
		cerr << "Error reading database: " << sqlite3_errmsg(dbh) << endl;
		sqlite3_free(sqlite3_error);
		return;
	}
}


// callback used by sqlite3_exec to print out a Customer's transactions
static int _db_view_transaction(void* unused, 
	int argc, char** argv, char** cols)
{
	char* timestamp = argv[1];
	char* type = argv[3];
	int amount = atoi(argv[4]);

	printf("%s: %s - $%.2f\n", timestamp, type, (float)amount / 100.0f);

	return 0;
}


// handles database interaction for updating the transaction log
void _update_transaction_log(Account* account, char* txType, int amount, 
	sqlite3* dbh)
{
	// get a useable timestamp from ctime (chop trailing newline)
	time_t now;
	time(&now);
	char* timestamp = ctime(&now);
	char* trailing_newline = strrchr(timestamp, '\n');
	*trailing_newline = '\0'; // chop it off

	// insert new transaction into the database
	stringstream sql;
	sql << "INSERT INTO transactions (timestamp, account_id, type, amount)"
		<< " VALUES('" << timestamp << "', "
		<< account->getAccountID() << ", "
		<< "'" << txType << "', "
		<< amount << ")";

	char* sqlite3_error;
	int rc = sqlite3_exec(dbh, sql.str().c_str(), NULL, NULL, &sqlite3_error);

	if (SQLITE_OK != rc) {
		cerr << "Error updating database: " << sqlite3_errmsg(dbh) << endl;
		sqlite3_free(sqlite3_error);
		return;
	}
}


// prompts a user to input a numeric value (either floating point or integer)
// if 'isMoney' is a true value, multiples the user input by 100 and converts
// the result to an integer.
static int _repeatedly_prompt_user_for_positive_numeric_input
	(const char* prompt, const char* error, int isMoney)
{
	int input = -1;
	do {
		cout << prompt;
		string buffer;
		getline(cin, buffer);

		if (isMoney) {
			double temp_input;
			if (! from_string<double>(temp_input, buffer, std::dec)) {
				input = -1;
			}
			else {
				input = (int)(temp_input * 100);
			}
		}
		else {
			if (! from_string<int>(input, buffer, std::dec)) {
				input = -1;
			}
		}

		if (input < 0 && NULL != error) {
			cout << error << endl;
		}
	} while(input < 0);

	return input;
}