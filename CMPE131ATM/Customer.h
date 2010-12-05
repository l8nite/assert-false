/**
 * @file Customer.h
 * @author assert-false@googlegroups.com
 * @date December 5th, 2010
 * @brief Customer class declaration
*/

#pragma once
#include "Account.h"
#include <vector>

/**
 * @class Customer
 *
 * The Customer class holds information about a customer.
 * This includes the customer ID, the list of Account objects the customer
 * owns, and a flag indicating whether or not the system has authorized the
 * customer.
 *
 * Example Usage:
 * @code
 *	   Customer* customer = new Customer();
 *     customer->setCustomerID(12345);
 *
 *     if ([authorize them somehow]) {
 *         customer->setIsAuthorized(1);
 *     }
 *
 *     Account* account = new Account();
 *     [fill in account details]
 *     
 *     customer->addAccount(account);
 *
 *	   vector<Account*> accounts = customer->getAccounts();
 *     for (int i = 0; i < accounts.size(); ++i) {
 *         accounts[i]->printSummary();
 *     }
 * @endcode
 */
class Customer
{
public:
	/**
	 * @name Constructors
	 */
	/**
	 * Generic Customer constructor
	 */
	Customer(void);
	
	/**
	 * Destructor
	 */
	virtual ~Customer(void);

public:
	/**
	 * Gets the list of Account instances that this customer holds
	 * @return vector<Account*> of the Account instances
	 */
	vector<Account*> getAccounts(void);

	/**
	 * Adds a new Account instance to this customer's accounts
	 * @param account a pointer to an Account instance
	 */
	void addAccount(Account* account);

	/**
	 * Gets the customer ID
	 * @return this customer's ID
	 */
	int getCustomerID(void);

	/**
	 * Sets the customer ID
	 * @param customerID the Customer's new ID
	 */
	void setCustomerID(int customerID);

	/**
	 * Returns whether or not this Customer is authorized
	 * @return an integer
	 * @retval 0 not authorized
	 * @retval !0 authorized
	 */
	int getIsAuthorized();

	/**
	 * Sets whether or not this Customer is authorized
	 * @param isAuthorized an integer representing whether or not this Customer
	 *  is authorized.  A value of zero means not authorized, otherwise authorized.
	 */
	void setIsAuthorized(int isAuthorized);

protected:
	//! the customer's ID
	int customer_id;

	//! whether or not they're authorized
	int is_authorized;

	//! the list of Account instances for this Customer
	vector<Account*> accounts;
};