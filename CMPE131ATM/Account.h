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

	// returns balance after withdrawal or deposit
	int withdrawFunds(int withdrawalAmount);
	int depositFunds(int depositAmount);

	// prints information about the account
	void printSummary();

protected:
	int account_id;
	int balance;
};