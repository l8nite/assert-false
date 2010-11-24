// CMPE131ATM.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include "Customer.h"

using namespace std;

// application logic
void loginCustomer();
void applicationMenuLoop();
void exitApplication(int exitCode);
void handleDeposit();
void handleWithdrawal();

// globals are ugly, but whatever.
Customer* gCustomer;

void main(int argc, _TCHAR* argv[])
{
	gCustomer = NULL;

	loginCustomer();
	applicationMenuLoop();
	exitApplication(0);
}

void loginCustomer() {
	// TODO: assert(gCustomer == NULL)
	cout << "Welcome to CMPE131 ATM" << endl;
	
	int account_id = 0;
	do {
    	cout << "Please input your account number: ";
		cin >> account_id;
	}
	while (!account_id); // TODO: validate account #

	gCustomer = new Customer(account_id);

	gCustomer->getAccount()->printSummary();
}

void applicationMenuLoop() {
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

	switch (menu_choice) {
	case 1:
		gCustomer->getAccount()->printSummary();
		break;
	case 2:
		gCustomer->getAccount()->printHistory();
		break;
	case 3:
		handleDeposit();	
		break;
	case 4:
		handleWithdrawal();
		break;
	case 5:
		exitApplication(0);
		break;
	}

	goto menu;
}

void exitApplication(int exitCode) {
	if (gCustomer) {
		delete gCustomer;
	}

	exit(exitCode);
}

void handleDeposit() {
	double depositAmount = 0.0;
	
	do {
    	cout << "Enter deposit amount: $";
    	cin >> depositAmount;
	}
	while (!depositAmount); // TODO validate amount
	
	// TODO: protect against overflowing double

	double balance = gCustomer->getAccount()->depositFunds(depositAmount);
	printf("Your new balance is: $%.2f\n", balance);
}

void handleWithdrawal() {
	double withdrawalAmount = 0.0;
	
	do {
    	cout << "Enter withdrawal amount: $";
    	cin >> withdrawalAmount;
	}
	while (!withdrawalAmount); // TODO validate amount

	// TODO: protect against overdrafting of the account

	double balance = gCustomer->getAccount()->withdrawFunds(withdrawalAmount);
	printf("Your new balance is: $%.2f\n", balance);
}
