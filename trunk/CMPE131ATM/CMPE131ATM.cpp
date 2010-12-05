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
Account* chooseAccount(Customer* customer, sqlite3* dbh);

// main menu
void applicationMenuLoop(Customer* customer, Account* account, sqlite3* dbh);

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

// callback used by sqlite3_exec to load an account for a customer
static int _db_load_account(void* customer,
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

	// log in and validate their pin
	Customer* customer = loginCustomer(dbh);

	if (NULL == customer) { // we couldn't log them in
		rc = -1;
		goto exit;
	}

	// choose which account they want to use 
	Account* account = chooseAccount(customer, dbh);

	// enter the main application loop
	applicationMenuLoop(customer, account, dbh);

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
	sql << "SELECT * FROM customers WHERE id=" << customer_id
		<< " AND pin=" << customer_pin;
	char* sqlite3_error;
	int rc = sqlite3_exec(dbh, sql.str().c_str(),
		_db_load_customer, customer, &sqlite3_error);

	if (SQLITE_OK != rc) {
		cerr << "Error reading customers: " << sqlite3_errmsg(dbh) << endl;
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

	// now load their accounts from the database
	sql.str("");
	sql << "SELECT * FROM accounts WHERE customer_id=" << customer_id
		<< " ORDER BY id ASC";
	rc = sqlite3_exec(dbh, sql.str().c_str(),
		_db_load_account, customer, &sqlite3_error);

	if (SQLITE_OK != rc) {
		cerr << "Error reading accounts: " << sqlite3_errmsg(dbh) << endl;
		sqlite3_free(sqlite3_error);
		delete customer;
		return NULL;
	}

	// we should always have an account for a customer, otherwise the database
	// has lost integrity.
	assert(customer->getAccounts().size() != 0);

	return customer;
}


// callback for database login routine 
static int _db_load_customer(void* customer, 
	int argc, char** argv, char** cols)
{
	// set the customer ID and authorize them
	((Customer*)customer)->setCustomerID(atol(argv[0]));
	((Customer*)customer)->setIsAuthorized(1);
	return 0;
}


// callback for database account loading routine
static int _db_load_account(void* customer,
	int argc, char** argv, char** cols)
{
	string label;
	Account* account = new Account();

	// load database row into the account/Customer object
	// argv = { id, customer_id, balance, label }
	account->setAccountID(atoi(argv[0]));
	account->setBalance(atoi(argv[2]));

	label += argv[3];
	account->setLabel(label);

	// add account to the customer object
	((Customer*)customer)->addAccount(account);

	return 0;
}


// if the user has more than one account, let them choose which one they
// want to work with
Account* chooseAccount(Customer* customer, sqlite3* dbh) {
	vector<Account*> accounts = customer->getAccounts();

	if (accounts.size() == 1) {
		return accounts[0];
	}

	menu:
	for (int i = 0; i < accounts.size(); ++i) {
		cout << (i+1) << ". #" << accounts[i]->getAccountID() << " - "
			 << accounts[i]->getLabel() << endl;
	}

	int menu_choice = _repeatedly_prompt_user_for_positive_numeric_input(
		"Choice? ", "Invalid choice.  Try again.", 0);

	if (menu_choice < 1 || menu_choice > accounts.size()) {
		cout << "Invalid choice.  Try again." << endl;
		goto menu;
	}

	return accounts[menu_choice-1];
}

// main menu loop
void applicationMenuLoop(Customer* customer, Account* account, sqlite3* dbh) {
	vector<Account*> accounts = customer->getAccounts();
	
	summary:
	account->printSummary();

	menu:
	cout << "1. Print Account Summary" << endl;
	cout << "2. View Transaction History" << endl;
	cout << "3. Deposit Funds" << endl;
	cout << "4. Withdraw Funds" << endl;
	if (accounts.size() > 1) {
		cout << "5. Change accounts" << endl;
		cout << "6. Exit" << endl;
	}
	else {
		cout << "5. Exit" << endl;
	}

	int menu_choice = _repeatedly_prompt_user_for_positive_numeric_input(
		"Choice? ", "Invalid choice.  Try again.", 0);

	if (menu_choice < 1 || menu_choice > (accounts.size() > 1 ? 6 : 5)) {
		cout << "Invalid choice.  Try again." << endl;
		goto menu;
	}

	switch (menu_choice) { // options from the menu above
	case 1:
		account->printSummary();
		break;
	case 2:
		handleTransactionHistory(account, dbh);
		break;
	case 3:
		handleDeposit(account, dbh);	
		break;
	case 4:
		handleWithdrawal(account, dbh);
		break;
	case 5:
		if (accounts.size() > 1) {
			account = chooseAccount(customer, dbh);
			goto summary;
		}
		else {
			return;
		}
		break;
	case 6:
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
	}else if (withdrawal_amount > 25000000){
		cout << "Your withdrawal exceeds FDIC insured limit or $250,000. Try again." << endl;
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
		<< " ORDER BY id DESC";

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