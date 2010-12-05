/**
 * @file Account.h
 * @author assert-false@googlegroups.com
 * @date December 5th, 2010
 * @brief Account class declaration
*/

#pragma once
#include <string>
#include <iostream>

using namespace std;

/**
 * @class Account
 *
 * The Account class holds information about an individual account record.
 * This includes the account ID, the current balance, and the account label.
 *
 * It also provides a convenient way to print the account summary to stdout.
 *
 * Example Usage:
 * @code
 *
 *     Account* account = new Account();
 *
 *     account->setAccountID(555);
 *     account->setBalance(5000); // $50
 *     account->setLabel("Checking");
 *
 *     account->printSummary();
 *
 * @endcode
 */
class Account
{
public:
	/**
	 * @name Constructors
	 */
	/**
	 * Generic Account constructor
	 */
	Account(void);

public:
	/** @name Accessor/Mutators
	 * These are the protected member accessor/mutators
	 */
	//@{

	/**
	 * Gets the account ID
	 * @return an integer representing the account ID
	 */
	int getAccountID(void);
	
	/** 
	 * Sets the account ID
	 * @param accountID the new accountID to set
	 */
	void setAccountID(int accountID);

	/**
	 * Gets the account balance
	 * @return an integer representing the account balance in cents
	 */
	int getBalance(void);

	/**
	 * Sets the account balance
	 * @param balance the account balance in cents
	 */
	void setBalance(int balance);

	/**
	 * Gets the account label
	 * @return a string for the account label
	 */
	string getLabel(void);

	/**
	 * Sets the account label
	 * @param label the new account label
	 */
	void setLabel(string label);
	//@}

	/**
	 * Prints a one-line summary of the account to stdout
	 */
	void printSummary();

protected:
	//! The account ID
	int account_id;

	//! The current balance (in cents)
	int balance;

	//! The account label (ie., "Checking" or "Savings")
	string label;
};