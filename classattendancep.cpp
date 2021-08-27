#include <iostream>
#include <stdio.h>
#include <windows.h>
#include "sqlite3.h"
#include <string>

using namespace std;

sqlite3* db;

class AttendanceSummary{
public:
	int displayHomePageOptions();
	void viewSemesterAttendance();
	string chooseSemesterToView();
};

class SignAttendance{
public:
	string signAttendance(char introAnswer);
	void flushClassRecords();
	int viewCurrentRecords();
	void deleteRecord(string regNo);
	void submitRecords(string semName);
};

class Admin 
{
public:
	void displayAdminHome();
	string removeSemesterUnit();
	string attendanceSummary();
	string newSemester();
	void saveSemesterName(string tableName, string unitName, string unitCode, string semYear, string semNo);
};

class Login : public Admin
{
public:
	void loginPage();
	void checkAdminCredentials(string serialNo,string password);
	void checkLecturerCredentials(string serialNo, string password);
};

string getPassFromUser();
static int callback_data(void* data, int argc, char** argv, char** azColName); // for printing record data
static int callback_columnNames(void* data, int argc, char** argv, char** azColName); // for printing column names


int main()
{
	string errmsg="", sqlStmt;
	int dbCommand;

	Login login;

	//Open Database
	dbCommand = sqlite3_open("adminfile.db", &db);

	if (dbCommand != SQLITE_OK)
	{
		errmsg = sqlite3_errmsg(db);
		std::cout<<"Can't open database: "+ errmsg << endl;
		return 0;
	}
	else
	{
		std::cout<<"Opened Database Successfully\n";
	}
	
	login.loginPage();

	//Close Database
	sqlite3_close(db);
	return 0;
}

int AttendanceSummary::displayHomePageOptions()
{
	char chosenOption;
	string backButton = "";

	std::cout << "\t\tWELCOME:\n";
	std::cout << "****************\n";
	std::cout << "Choose one of the following options : \n";
	std::cout << "***************\n";
	std::cout << "\t1. Sign Attendance\n";
	std::cout << "\t2. View Attendance Summary\n\n";
	std::cout << "\t0. Exit\n";
	std::cout << "***************\n\n";

	std::cin >> chosenOption;

	switch (chosenOption)
	{
	case '0':
		system("cls");
		Login login;
		login.loginPage();
		break;
	case '1':
		system("cls");
		SignAttendance sign;
		sign.signAttendance('0');

		// back button-like command
		system("cls");
		displayHomePageOptions();
		break;
	case '2':
		system("cls");
		viewSemesterAttendance();

		// back button-like command
		std::cout << "Enter Any Character To Go Back...\n";
		std::cout << "***************\n";
		std::cin >> backButton;
		if (backButton == "0")
		{
			system("cls");
			displayHomePageOptions();
			return chosenOption;
		}
		else
		{
			system("cls");
			displayHomePageOptions();
			return chosenOption;
		}
		break;
	default:
		system("cls");
		std::cout << "***************\n\n";
		std::cout << "You Entered " << chosenOption << endl;
		std::cout << "INVALID OPTION...\n";
		std::cout << "***************\n\n";
		displayHomePageOptions();
		break;
	}
	return chosenOption;
}

string AttendanceSummary::chooseSemesterToView()
{
	string sqlSelect="", columnName="", chosenSemester="";
	string unitCode="", semYear="", semNo="";
	int dbCommand;

	std::cout << "**************************\n";
	std::cout << "\t\tEnter 0 To Go Back...\n";
	std::cout << "**************************\n\n";

	// Get list of available Semesters
	sqlSelect = "SELECT * FROM academicYearSemesters ORDER BY SemesterName DESC;";
	dbCommand = sqlite3_exec(db, sqlSelect.c_str(), callback_data, NULL, NULL);

	std::cout << "**************************\n";
	std::cout << "Enter The Unit Code: (i.e. EEE202)\n";
	std::cout << "***************\n";
	getline(cin, unitCode);

	if (unitCode == "0")
	{
		system("cls");
		return unitCode;
	}

	std::cout << "***************\n";
	std::cout << "Academic Year : \n";
	std::cout << "***************\n";
	getline(cin, semYear);

	if (semYear == "0")
	{
		system("cls");
		return semYear;
	}

	std::cout << "***************\n";
	std::cout << "Enter The Semester: (i.e. 1, 2, ...)\n";
	std::cout << "***************\n";
	getline(cin, semNo);

	if (semNo == "0")
	{
		system("cls");
		return semNo;
	}
	std::cout << "***************\n";

	chosenSemester = unitCode + "" + semYear + "" + semNo + "_";
	
	return chosenSemester;
}

void AttendanceSummary::viewSemesterAttendance()
{
	string sqlStmt="", columnName="", chosenSemester="";
	int dbCommand;

	if(chosenSemester != "0")
	{
		chosenSemester = chooseSemesterToView();

		sqlStmt = "SELECT * FROM " + chosenSemester +" ORDER BY admNo;";
		columnName = "SELECT * FROM " + chosenSemester + " WHERE ListNumber = 1";

		std::cout << "                                SUMMARY OF ATTENDANCE FOR THE SEMESTER\n";
		std::cout << "******************************************\n";

		//Execute sql statement 

		dbCommand = sqlite3_exec(db, columnName.c_str(), callback_columnNames, NULL, NULL);
		dbCommand = sqlite3_exec(db, sqlStmt.c_str(), callback_data, NULL, NULL);

		if (dbCommand != SQLITE_OK)
		{
			system("cls");
			std::cout << "Please Try Again Later...\n";
			displayHomePageOptions();
		}
	}
}

string SignAttendance:: signAttendance(char introAnswer)
{
	string option = "1", sqlStmt="", unitCode="", semYear="", semNo="", semDetails="";
	int count = 0, dbCommand;

	if(introAnswer == '0')
	{
		std::cout << "****************\n";
		std::cout <<"Begin New Class ?  Y / N \n";
		std::cout << "****************\n\n";
		std::cin >> introAnswer;
		system("cls");
	}
	if(introAnswer == 'Y' ||introAnswer == 'y')
	{
		std::cout << "************************\n";
		std::cout << "This Will Delete Information From Previous Class, Continue ? Y / N ... \n";
		std::cout << "*************************\n\n";
		std::cin >> introAnswer;

		if(introAnswer == 'Y' || introAnswer == 'y')
		{
			system("cls");
			flushClassRecords();
		}
	}
	std::cout << "***************\n";
	std::cout << "\t\tEnter 0 to Go Back\n";
	std::cout << "****************\n\n";
	std::cout << "Available Semesters : \n";
	std::cout << "________\n";

	sqlStmt = "SELECT * FROM academicYearSemesters ORDER BY SemesterName DESC;";
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), callback_data, NULL, NULL);

	std::cout << "***************\n";
	std::cout << "Enter The Unit Code: (i.e. EEE202)\n";
	std::cout << "***************\n";
	cin.ignore();
	getline(std::cin,unitCode);

	if (unitCode == "0")
	{
		return unitCode;
	}

	std::cout << "***************\n";
	std::cout<<"Academic Year:\n";
	std::cout<<"****************\n";
	getline(std::cin, semYear);

	if (semYear == "0")
	{
		return semYear;
	}

	std::cout << "***************\n";
	std::cout << "Enter The Semester: (i.e. 1, 2, ...)\n";
	std::cout << "***************\n";
	getline(std::cin, semNo);

	if (semNo == "0")
	{
		return semNo;
	}

	semDetails = unitCode + "" + semYear + "" + semNo + "_";

	system("cls");
	std::cout << "***************\n";
	std::cout << "Number of Students Enrolled:\n";
	std::cout << "*****************\n";

	sqlStmt = "SELECT COUNT(ListNumber) FROM " + semDetails;
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), callback_data, NULL, NULL);

	std::cout << "*****************\n";

	if (dbCommand != SQLITE_OK)
	{
		system("cls");
		std::cout << semDetails;
		std::cout << "ENTRY DOES NOT EXIST...TRY AGAIN LATER...\n";
		std::cout << "*****************\n";
		signAttendance('1');
		option = "0"; // to prevent entering into while loop after function execution ends
	}

	system("cls");
	while (option != "0")
	{
		count = count + 1;
		std::cout << "Sign Attendance for: "<< semDetails <<endl;
		std::cout << "****************\n";
		std::cout << "\t*\t0 - Go Back To Main Menu\n";
		std::cout << "\t*\t1 - View Current Class List \n";
		std::cout << "\t*\t2 - Submit Class Attendance \n";
		std::cout << "*******************\n\n";
		std::cout << "Enter Admission Number or Option : \n";
		std::cout << "*******************\n";
		getline(cin, option);
		std::cout << "*******************\n";
		

		if(option == "0")
		{
			return option;
		}
		else if(option == "1")
		{
			system("cls");
			viewCurrentRecords();
			return option;
		}
		else if(option == "2")
		{
			system("cls");
			submitRecords(semDetails);
			return option;
		}
		else // process admission number
		{
			sqlStmt = "INSERT INTO classRecords SELECT * FROM studentRecords WHERE RegNo = '" + option + "';";
			dbCommand = sqlite3_exec(db,sqlStmt.c_str(), 0, NULL, NULL);

			sqlStmt = "SELECT Name FROM studentRecords WHERE RegNo = '"+option+"'";
			dbCommand = sqlite3_exec(db, sqlStmt.c_str(),callback_data, NULL, NULL); 

			if(dbCommand != SQLITE_OK)
			{
				std::cout << "INVALID ENTRY............\n \
				      TRY AGAIN LATER......\n \
			***************\n\n";
			}
		}
	}
	return semDetails;
}

void SignAttendance :: flushClassRecords()
{
	string sqlStmt="";
	int dbCommand;

	sqlStmt = "DELETE FROM classRecords;";
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), 0, NULL, NULL);
	if(dbCommand != SQLITE_OK)
	{
		std::cout << "***************\n";
		std::cout << "An Error Ocurred Please Try Again Later...\n";
		std::cout << "Records Not Cleared Successfully.... \n";
		std::cout << "***************\n\n";
	}
}

int SignAttendance :: viewCurrentRecords()
{
	string sqlStmt="", regNo="";
	int dbCommand;
	char option;

	std::cout << "CLASS ATTENDANCE LIST :\n";
	std::cout<<"****************\n";

	sqlStmt = "SELECT * FROM classRecords ORDER BY RegNo;";
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), callback_data, NULL, NULL);

	std::cout << "****************\n";
	std::cout << "Choose An Option :\n ";
	std::cout << "\t 1. Delete An Entry\n ";
	std::cout << "\t 0. Go Back \n ";
	std::cout << "***************\n\n";
	std::cin >> option;

	if(option == '1')
	{
		std::cout << "Enter The Admission Number of Record You Want to Remove...\n ";
		std::cout << "i.e. J23/0000/2019 \n";
		std::cout << "********************\n";
		std::cin >> regNo;
		std::cin.ignore();
		system("cls");
		deleteRecord(regNo);
		viewCurrentRecords();
		return option;
	}
	else if(option == '0')
	{
		system("cls");
		signAttendance('1');
		return option;
	}
	else
	{
		system("cls");
		std::cout <<"' " << option << " ' IS AN INVALID OPTION...\n";
		std::cout << "****************\n";
		viewCurrentRecords();
	}
	return option;
}

void SignAttendance :: deleteRecord(string regNo)
{
	string sqlStmt="";
	int dbCommand;

	sqlStmt = "DELETE FROM classRecords WHERE RegNo = '" + regNo + "';";
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), 0, NULL, NULL);
}
void SignAttendance :: submitRecords(string semName)
{
	string sqlStmt="";
	int dbCommand;

	sqlStmt = "UPDATE "+ semName +" SET TotalClasses = TotalClasses + 1";
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), 0, NULL, NULL);

	sqlStmt = "UPDATE " + semName + " SET classesAttended = classesAttended + 1 WHERE EXISTS (SELECT RegNo FROM classRecords WHERE "+ semName +".admNo = classRecords.RegNo);";
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), 0, NULL, NULL);

	sqlStmt = "UPDATE " + semName + " SET PercentageAttendance = (100 * classesAttended / TotalClasses;";
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), 0, NULL, NULL);

	sqlStmt = "UPDATE "+semName +" SET ExamEligibility = 'N' WHERE PercentageAttendance < 70;";
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), 0, NULL, NULL);
}

void Admin::displayAdminHome()
{
	char optionChosen;
	string backButton = "";

	Login login;

	std::cout << "\tATTENDANCE SUMMARY\n";
	std::cout << "*************\n";
	std::cout << "Choose an Option: \n";
	std::cout << "*************\n";
	std::cout << "0. Go Back\n";
	std::cout << "1. View Attendance Summary\n";
	std::cout << "2. Prepare Unit For New Semester\n";
	std::cout << "3. Remove Semester Unit\n";
	std::cout << "*************\n\n";
	std::cin >> optionChosen;
	std::cin.ignore();

	switch (optionChosen)
	{
	case '0':
		system("cls");
		login.loginPage();
		break;

	case '1':
		system("cls");
		std::cout << "Option 1  Selected...\n";
		attendanceSummary();

		// back button-like command
		std::cout << "\nEnter 0 to Exit and Go Back to Previous Page...\n";
		std::cin >> backButton;
		if (backButton == "0")
		{
			system("cls");
			displayAdminHome();
		}
		else
		{
			system("cls");
			displayAdminHome();
		}
		break;

	case '2':
		system("cls");
		std::cout << "Option 1  Selected...\n";
		newSemester();
		
		// back button-like command
		std::cout << "Enter 0 to Exit and Go Back to Previous Page...\n";
		std::cin >> backButton;
		if (backButton == "0")
		{
			system("cls");
			displayAdminHome();
		}
		break;

	case '3':
		system("cls");
		removeSemesterUnit();
		system("cls");
		displayAdminHome();
		break;

	default:
		system("cls");
		std::cout << "Enter A Valid Option....\n";
		displayAdminHome();
		break;
	}
}

string Admin :: removeSemesterUnit()
{
	string sqlStmt="", semName="", unitCode="", semYear="", semNo="";
	int dbCommand;

	std::cout << "***************\n";
	std::cout << "\t\tEnter 0 to Go Back\n";
	std::cout << "***************\n\n";
	std::cout << "Available Semesters:\n";
	std::cout << "________\n";

	sqlStmt = "SELECT * FROM academicYearSemesters ORDER BY SemesterName DESC;";
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), callback_data, NULL, NULL);

	std::cout << "***************\n";
	std::cout << "Enter The Unit Code: (i.e. EEE202)\n";
	std::cout << "***************\n";
	getline(std::cin, unitCode);

	if (unitCode == "0")
	{
		return unitCode;
	}

	std::cout << "***************\n";
	std::cout << "Academic Year:\n";
	std::cout << "***************\n";
	getline(std::cin, semYear);

	if (semYear == "0")
	{
		return semYear;
	}

	std::cout << "***************\n";
	std::cout << "Enter The Semester: (i.e. 1, 2, ...)\n";
	std::cout << "***************\n";
	getline(std::cin, semNo);

	if (semNo == "0")
	{
		return semNo;
	}
	std::cout << "***************\n";

	semName = unitCode + "" + semYear + "" + semNo + "_";

	sqlStmt = "DROP TABLE "+ semName;
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), 0, NULL, NULL);

	sqlStmt = "DELETE FROM academicYearSemesters WHERE SemesterName = '"+ semName +"';";
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), 0, NULL, NULL);

	if (dbCommand == SQLITE_OK)
	{
		system("cls");
		std::cout << "***************\n";
		std::cout << "Unit "<<semName<<" Removed Successfully...\n";
		std::cout << "***************\n";
	}
	else
	{
		system("cls");
		std::cout << "***************\n";
		std::cout << "AN ERROR OCCURRED PLEASE TRY AGAIN LATER...\n";
		std::cout << "***************\n";
	}
	return semName;
}

string Admin:: attendanceSummary()
{
	string unitCode="", semYear="", semNo="", sqlStmt="", chosenSemester="";
	int dbCommand;

	std::cout << "**************************\n";
	std::cout << "\t\tEnter 0 To Go Back...\n";
	std::cout << "**************************\n";
	std::cout<<"Available Semesters : \n";
	std::cout<<"________\n";

	// Get list of available Semesters
	sqlStmt = "SELECT * FROM academicYearSemesters ORDER BY SemesterName DESC;";
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), callback_data, NULL, NULL);

	std::cout << "**************************\n";
	std::cout<<"Enter The Unit Code: (i.e. EEE202)\n";
	std::cout<<"*****************\n";
	getline(std::cin, unitCode);

	if(unitCode == "0")
	{
		return unitCode;
	}

	std::cout << "***************\n";
	std::cout << "Academic Year:\n";
	std::cout << "***************\n";
	getline(std::cin, semYear);

	if (semYear == "0")
	{
		return semYear;
	}

	std::cout << "***************\n";
	std::cout << "Enter The Semester: (i.e. 1, 2, ...)\n";
	std::cout << "***************\n";
	getline(std::cin ,semNo);

	if (semNo == "0")
	{
		return semNo;
	}

	std::cout << "***************\n";

	chosenSemester = unitCode + "" + semYear + "" + semNo + "_";

	sqlStmt = "SELECT * FROM " + chosenSemester + " ORDER BY admNo;";

	std::cout << "				    SUMMARY OF ATTENDANCE FOR THE SEMESTER\n \
	*******************************************\n";

	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), callback_data, NULL, NULL);

	if (dbCommand != SQLITE_OK)
	{
		system("cls");
		std::cout << "***************\n";
		std::cout << "An Error Occurred Please Try Again Later...\n";
		std::cout << "***************\n";
	}
	return chosenSemester;
}

void Admin::saveSemesterName(string tableName, string unitName, string unitCode, string semYear, string semNo)
{
	string sqlStmt="";
	int dbCommand;

	sqlStmt = "INSERT INTO academicYearSemesters (SemesterName, UnitCode, UnitName, AcademicYear,Semester) VALUES('" + tableName + "', '" + unitCode + "', '" + unitName + "', '" + semYear + "', " + semNo + ");";
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), 0, NULL, NULL);

	if (dbCommand != SQLITE_OK)
	{
		std::cout << "***************\n";
		std::cout << "An Error Occurred Please Try Again Later...\n";
		std::cout << "***************\n";
	}
	else
	{
		std::cout << "*************\n";
		std::cout << "New Semester Saved Successfully...\n";
		std::cout << "*************\n";
	}
}

string Admin::newSemester()
{
	string sqlStmt="", unitName = "", unitCode="", semYear="", semName="", semNo="";
	int dbCommand;
	std::cout << "\tPREPARING UNIT IN NEW SEMESTER\n";
	std::cout << "*****************************\n";
	std::cout << "\t\tEnter 0 to Go Back\n";
	std::cout << "*****************************\n";
	std::cout << "Enter The Unit Name : (i.e.COMPUTER PROGRAMMING 2)\n";
	std::cout << "***************\n";

	getline(std::cin, unitName);
	if (unitName == "0")
	{
		return unitName;
	}

	std::cout << "***************\n";
	std::cout << "Enter The Unit Code: (i.e. EEE202)\n";
	std::cout << "***************\n";
	getline(std::cin, unitCode);

	if (unitCode == "0")
	{
		return unitCode;
	}

	std::cout << "***************\n";
	std::cout << "Academic Year:\n";
	std::cout << "***************\n";
	getline(std::cin,semYear);

	if (semYear == "0")
	{
		return semYear;
	}

	std::cout << "***************\n";
	std::cout << "Enter The Semester: (i.e. 1, 2, ...)\n";
	std::cout << "***************\n";
	getline(std::cin, semNo);

	if (semNo == "0")
	{
		return semNo;
	}

	std::cout << "*****************\n";
	std::cout << "Preparing " << unitName << " for new Semester....";
	std::cout << "*****************\n";

	semName = unitCode + "" + semYear + "" + semNo + "_";

	sqlStmt = "CREATE TABLE " + semName + " (ListNumber INTEGER PRIMARY KEY, admNo TEXT, Name TEXT, classesAttended INTEGER, TotalClasses INTEGER, PercentageAttendance INTEGER, ExamEligibility TEXT);";
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), 0, NULL, NULL);

	sqlStmt = "INSERT INTO " + semName + "(admNo, Name) SELECT RegNo,Name FROM studentRecords WHERE RegNo<>'0'";
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), 0, NULL, NULL);

	sqlStmt = "UPDATE " + semName + " SET classesAttended = 0";
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), 0, NULL, NULL);

	sqlStmt = "UPDATE " + semName + " SET TotalClasses = 0";
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), 0, NULL, NULL);

	sqlStmt = "UPDATE " + semName + " SET PercentageAttendance = 1";
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), 0, NULL, NULL);

	sqlStmt = "UPDATE " + semName + " SET ExamEligibility = 'Y'";
	dbCommand = sqlite3_exec(db, sqlStmt.c_str(), 0, NULL, NULL);

	if (dbCommand != SQLITE_OK)
	{
		std::cout << "*****************\n";
		std::cout << "An Error Occurred Please Try Again Later...\n";
		std::cout << "*****************\n";
	}
	else
	{
		std::cout << "*************\n";
		std::cout << "New Semester Created Successfully...\n";
		std::cout << "*************\n";
	}

	saveSemesterName(semName, unitName, unitCode, semYear, semNo);
	return semName;
}

void Login :: loginPage()
{
	string serialNo="", password="";
	char userType;
	
	std::cout << "*********************\n";
	std::cout << "WELCOME TO THE CLASS ATTENDANCE INFORMATION MANAGEMENT SYSTEM\n";
	std::cout << "*********************\n";
	std::cout << "Select User Type: \n";
	std::cout << "*************\n";
	std::cout << "1. Lecturer\n";
	std::cout << "2. Admin\n";
	std::cout << "*************\n";
	std::cout << "\t\t\t|********\n";
	std::cout << "\t\t\t|Enter 0 To Exit:\n";
	std::cout << "\t\t\t|********\n";
	std::cin >> userType;

	system("cls");
	std::cin.ignore();

	switch (userType)
	{
	case '0':
		system("cls");
		std::cout << "Program Terminated\n";
		break;
	case '1':
		std::cout << "Enter Your Lecturer Serial Number : \n";
		std::cout << "*************\n";
		getline(cin, serialNo);
		std::cout << "*************\n";
		std::cout << "Enter Your Password : \n";
		std::cout << "*************\n";
		password = getPassFromUser();

		checkLecturerCredentials(serialNo, password);
		system("cls");
		break;
	case '2':
		std::cout << "Enter Your Admin Serial Number : \n";
		std::cout << "*************\n";
		getline(cin, serialNo);
		std::cout << "*************\n";
		std::cout << "Enter Your Password : \n";
		std::cout << "*************\n";
		password = getPassFromUser();

		checkAdminCredentials(serialNo, password);
		system("cls");
		break;
	default:
		system("cls");
		std::cout << "*******\n";
		std::cout << "INVALID INPUT....\n";
		std::cout << "*******\n";
		loginPage();
		break;
	}
}

void Login :: checkAdminCredentials(string serialNo,string password)
{
	string sqlStmt = "";
	sqlite3_stmt* stmt;
	int dbCommand;

	sqlStmt = "SELECT Name FROM adminRecords WHERE SerialNo = '" + serialNo + "' AND Password = '" + password + "';";
	dbCommand = sqlite3_prepare_v2(db, sqlStmt.c_str(), -1, &stmt, NULL);//required in order to use sqlite3_step
	dbCommand = sqlite3_step(stmt);//returns SQLITE_ROW when successful

	if (dbCommand != SQLITE_ROW)
	{
		system("cls");
		std::cout << "***************\n";
		std::cout << "INVALID CREDENTIALS...TRY AGAIN LATER...\n";
		std::cout << "***************\n";
		sqlite3_finalize(stmt); // to end the sqlite3 prepare
		loginPage();
	}
	else
	{
		system("cls");
		sqlite3_finalize(stmt); // to end the sqlite3 prepare
		Admin admin;
		admin.displayAdminHome();
	}
}

void  Login :: checkLecturerCredentials(string serialNo, string password)
{
	string sqlStmt = "";
	sqlite3_stmt* stmt;
	int dbCommand;

	sqlStmt = "SELECT Name FROM lecturerRecords WHERE SerialNo = '" + serialNo + "' AND Password = '" + password + "';";
	dbCommand = sqlite3_prepare_v2(db, sqlStmt.c_str(), -1, &stmt, NULL); //required in order to use sqlite3_step
	dbCommand = sqlite3_step(stmt); //returns SQLITE_ROW when successful

	if (dbCommand != SQLITE_ROW)
	{
		system("cls");
		std::cout << "***************\n";
		std::cout << "INVALID CREDENTIALS...TRY AGAIN LATER...\n";
		std::cout << "***************\n";
		sqlite3_finalize(stmt); // to end the sqlite3 prepare
		loginPage();
	}
	else
	{
		system("cls");
		sqlite3_finalize(stmt);// to end the sqlite3 prepare
		AttendanceSummary summary;
		summary.displayHomePageOptions();
	}
}

string getPassFromUser()
{
	HANDLE hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;

	// create a restore point Mode
	//is now 503
	GetConsoleMode(hStdInput, &mode);

	//enable echo input
	//set to 499
	SetConsoleMode(hStdInput, mode & (~ENABLE_ECHO_INPUT));

	//take input
	string pass;
	getline(cin, pass);
	cout << endl;

	//Restore the mode
	SetConsoleMode(hStdInput, mode);

	return pass;
}

static int callback_data(void *data, int argc, char **argv, char **azColName)
{
	int i;

	for (i = 0; i < argc; i++)
	{
		printf("%s\t", argv[i] ? argv[i] : "NULL");
	}
	
	printf("\n");
	return 0;
}

static int callback_columnNames(void* data, int argc, char** argv, char** azColName)
{
	int i;
	for (i = 0; i < argc; i++)
	{
		printf("%s\t", azColName[i]);
	}

	printf("\n");
	return 0;
}