#pragma once
//Acccount -- contains account information for each customer.
//@author Assert-False

class Account
{
public:
	//class constructor
	//@param none
	//@return none
	Account(void);

public:
	//gets AccountID value
	//@param none
	//@retun AccountID
	int getAccountID(void);

	//sets AccountID value
	//@param accountID
	//@return none
	void setAccountID(int accountID);

	//gets balance value
	//@param none
	//@retun balance
	int getBalance(void);

	//sets balance value
	//@param balance
	//@return none
	void setBalance(int balance);

	//prints account information to the screen
	//@param none
	//@return none
	void printSummary();

protected:
	int account_id;
	int balance; //balance: kept in integer form. $1000.00 is equal to 100000
};