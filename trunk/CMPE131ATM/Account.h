#pragma once
class Account
{
public:
	Account(void);
	Account(int customerID);

public:
	int getCustomerID();
	void setCustomerID(int customerID);

	double getBalance ();
	void setBalance(double balance);

	// returns balance after withdrawal or deposit
	double withdrawFunds(double withdrawalAmount);
	double depositFunds(double depositAmount);

	// prints information about the account
	void printSummary();
	void printHistory();

protected:
	int customer_id;
	double balance;

};