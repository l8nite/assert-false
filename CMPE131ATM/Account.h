#pragma once
class Account
{
public:
	Account(void);

public:
	int getAccountID(void);
	void setAccountID(int accountID);

	int getBalance(void);
	void setBalance(int balance);

	// prints information about the account
	void printSummary();

protected:
	int account_id;
	int balance;
};