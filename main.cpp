// HEADER FILES

#include <iostream>
#include <conio.h>
#include <mysql.h>
#include <sstream>

#include "config.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

using namespace std;

// GLOBAL VARIABLES

MYSQL* conn;
stringstream stmt;
string query;
const char* q;
MYSQL_RES* res_set;
MYSQL_ROW row;

// CLASSES MADE

class books
{
    int id{};
    string name;
    string auth;
    int price{};
    int qty{};

public:
    void add();
    void update_price();
    void search();
    static void update();
    static void display();
};

// MEMBER FUNCTIONS


//class books

/**
 * @brief Add a new book record to the database.
 *
 * This function prompts the user to enter the name, author, price, and quantity of a book,
 * and inserts the corresponding values into the "Books" table in the database.
 *
 * @details This function first prompts the user to enter the name of the book, author's name,
 * price, and quantity received using cin and getline to capture strings that may contain spaces.
 * It then initializes a statement handler using mysql_stmt_init() and prepares a query to insert
 * values into the "Books" table using mysql_stmt_prepare(). The values to be inserted are bound to
 * the statement using MYSQL_BIND structures. The name and author are bound as strings, while the
 * price and quantity are bound as integers. The statement is executed using mysql_stmt_execute(),
 * and if successful, a confirmation message is printed. Finally, the statement handler is closed
 * using mysql_stmt_close().
 *
 * @param None
 * @return None
 */
void books::add()
{
    cout << "Enter the name of the book: ";
    cin.ignore();
    getline(cin, name);
    cout << "Enter the name of the author: ";
    getline(cin, auth);
    cout << "Enter the Price: ";
    cin >> price;
    cout << "Enter the Qty Received: ";
    cin >> qty;

    MYSQL_STMT* mysqlStmt = mysql_stmt_init(conn);
    if (!mysqlStmt)
    {
        cout << "Could not initialize statement handler\n";
        return;
    }

    if (const auto str = "INSERT INTO Books (name, auth, price, qty) VALUES (?, ?, ?, ?)"; mysql_stmt_prepare(
        mysqlStmt, str, strlen(str)))
    {
        cout << "Could not prepare statement\n" << mysql_stmt_error(mysqlStmt) << "\n";
        return;
    }

    MYSQL_BIND bind[4] = {};

    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = const_cast<char*>(name.c_str());
    bind[0].buffer_length = name.length();

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = const_cast<char*>(auth.c_str());
    bind[1].buffer_length = auth.length();

    bind[2].buffer_type = MYSQL_TYPE_LONG;
    bind[2].buffer = reinterpret_cast<char*>(&price);
    bind[2].buffer_length = sizeof(price);

    bind[3].buffer_type = MYSQL_TYPE_LONG;
    bind[3].buffer = reinterpret_cast<char*>(&qty);
    bind[3].buffer_length = sizeof(qty);

    if (mysql_stmt_bind_param(mysqlStmt, bind))
    {
        cout << "Could not bind parameters\n";
        return;
    }

    if (mysql_stmt_execute(mysqlStmt))
    {
        cout << "Could not execute prepared statement\n" << mysql_stmt_error(mysqlStmt) << "\n";
        return;
    }

    cout << "Book record inserted successfully\n";

    mysql_stmt_close(mysqlStmt);
}

/**
 * @brief Updates the price of a book based on the given book ID.
 *
 * This function prompts the user to enter a book ID and fetches the
 * current name and price from the database. It then asks the user
 * whether they want to update the price of the book. If the user
 * confirms, it prompts for a new price and updates the database
 * accordingly. If the user cancels, it does nothing. If the book
 * with the given ID does not exist, it displays a message indicating
 * that no book was found.
 *
 * @param None
 * @return None
 */
void books::update_price()
{
    cout << "Enter the id of the book for update in price : ";
    cin >> id;
    stmt.str("");
    stmt << "Select name, price from Books where id = " << id << ";";
    query = stmt.str();
    q = query.c_str();

    if (mysql_query(conn, q))
    {
        cout << "Query Error!" << endl;
        return;
    }

    MYSQL_RES* res_set = mysql_store_result(conn);
    if (MYSQL_ROW row; (row = mysql_fetch_row(res_set)) != nullptr)
    {
        char choice;
        cout << "The name of the book is : " << row[0] << endl;
        cout << "The current price of the book is : " << row[1] << endl;
        cout << "Do you want to upgrade the price [y/n] : ";
        cin >> choice;

        if (tolower(choice) == 'y')
        {
            cout << "Enter the new price : ";
            cin >> price;
            stmt.str("");
            stmt << "Update Books set price = " << price << " where id = " << id << ";";
            query = stmt.str();
            q = query.c_str();
            if (mysql_query(conn, q))
            {
                cout << endl << endl << "Query Error!" << endl << "Contact technical team" << endl << endl << endl;
            }
            else
            {
                cout << endl << endl << "Book price updated successfully" << endl << endl << endl;
            }
        }
        else
        {
            cout << "No changes made!";
        }
    }
    else
    {
        cout << "No book found!";
    }
}

/**
 * @brief Performs a search for a book record in the database.
 *
 * This function prompts the user to enter the book ID and searches for a corresponding record
 * in the "Books" table in the database. If a matching record is found, the details of the book
 * (including the name, author, price, and quantity) are displayed. If no matching record is found,
 * a message indicating "No record found" is displayed.
 *
 * @details This function first prompts the user to enter the book ID using cin. The function then
 * constructs a query string to search for the book record in the "Books" table based on the provided
 * book ID. The query string is executed using the mysql_query() function, and the result set is stored
 * using mysql_store_result(). The function then retrieves the first row of the result set using
 * mysql_fetch_row(). If a row is retrieved (indicating a matching record was found), the details of
 * the book (including the book ID, name, author, price, and quantity) are printed to the console using
 * cout. If no row is retrieved (indicating no matching record was found), a message indicating "No record
 * found" is displayed.
 *
 * @param None
 * @return None
 **/
void books::search()
{
    cout << "Enter book id for details : ";
    cin >> id;
    stmt.str("");
    stmt << "Select * from Books where id = " << id << ";";
    query = stmt.str();
    q = query.c_str();

    if (mysql_query(conn, q))
    {
        cout << "Query Error!" << endl;
        return;
    }

    MYSQL_RES* res_set = mysql_store_result(conn);
    if (MYSQL_ROW row; (row = mysql_fetch_row(res_set)) != nullptr)
    {
        cout << "The Details of Book Id " << row[0] << endl;
        cout << "The Name of the book is : " << row[1] << endl;
        cout << "The Author of " << row[1] << " is " << row[2] << endl;
        cout << "The Price ofthe book is : " << row[3] << endl;
        cout << "The inventory count is " << row[4] << endl;
    }
    else
    {
        cout << "No record Found" << endl;
    }
}

/**
 * @brief Update the quantity of books in the database.
 *
 * This function updates the quantity of books in the "Books" table in the database based on
 * the records in the "Purchases" table where the books have been received but not yet invoiced.
 * It retrieves the book IDs and quantities from the "Purchases" table and uses them to update
 * the corresponding book records in the "Books" table.
 *
 * @details This function first retrieves the book IDs and quantities from the "Purchases" table
 * where the books have been received but not yet invoiced. It executes a SELECT query to fetch the
 * book_id and qty columns from the "Purchases" table using mysql_query() and mysql_store_result()
 * functions. If the query execution or result storing fails, an error message is printed and the
 * function returns.
 *
 * Then, it executes an UPDATE query to set the inv column of the "Purchases" table to 1 for the
 * received but not yet invoiced books. Again, mysql_query() is used to execute the query, and if
 * it fails, an error message is printed and the function returns.
 *
 * Next, it fetches the book_id and qty values from each row of the result set using mysql_fetch_row()
 * function. The book_id and qty values are converted to integer using atoi() function and stored
 * in the b_id[] and qty[] arrays respectively. The arrays are used to update the quantity of
 * corresponding book records in the "Books" table.
 *
 * Finally, an UPDATE query is executed for each book ID and quantity in the b_id[] and qty[] arrays
 * using mysql_query() function. If any of the queries fail, an error message is printed and the
 * function returns. Otherwise, a success message is printed.
 *
 * @param None
 * @return None
 */
void books::update()
{
    int b_id[100], qty[100], i = 0;
    stmt.str("");
    stmt << "Select book_id,qty from Purchases where received = 'T' and inv IS NULL;";
    query = stmt.str();
    q = query.c_str();

    if (mysql_query(conn, q))
    {
        cout << "Failed to execute query: " << mysql_error(conn) << endl;
        return;
    }

    res_set = mysql_store_result(conn);
    if (res_set == nullptr)
    {
        cout << "Failed to store result: " << mysql_error(conn) << endl;
        return;
    }

    stmt.str("");
    stmt << "Update Purchases set inv = 1 where received = 'T' and inv IS NULL;";
    query = stmt.str();
    q = query.c_str();

    if (mysql_query(conn, q))
    {
        cout << "Failed to execute query: " << mysql_error(conn) << endl;
        return;
    }

    while ((row = mysql_fetch_row(res_set)) != nullptr)
    {
        b_id[i] = atoi(row[0]);
        qty[i] = atoi(row[1]);
        i++;
    }

    const int max = i;
    for (i = 0; i <= max; i++)
    {
        stmt.str("");
        stmt << "update Books set qty = " << qty[i] << " where id = " << b_id[i] << ";";
        query = stmt.str();
        q = query.c_str();

        if (mysql_query(conn, q))
        {
            cout << "Failed to execute query: " << mysql_error(conn) << endl;
            return;
        }
    }

    cout << "The orders recieved have been updated.";
}

void books::display()
{
    int i = 0;
    query = "Select * from Books;";
    q = query.c_str();
    mysql_query(conn, q);
    res_set = mysql_store_result(conn);

    while ((row = mysql_fetch_row(res_set)) != nullptr)
    {
        cout << endl;
        cout << "Name for book " << ++i << " : " << row[1] << endl;
        cout << "Name of Author : " << row[2] << endl;
        cout << "Price : " << row[3] << endl;
        cout << "Quantity : " << row[4] << endl;
    }
}
//class suppliers

//class purchased

//class employees

//class members

//class sales

// FUNCTIONS

void book_menu();

//main menu


void main_menu()
{
    int c;
    cout << "*************************************************" << endl;
    cout << "         BOOKSHOP MANAGEMENT SYSTEM" << endl;
    cout << "*************************************************" << endl;
    cout << "   1. BOOKS" << endl;
    cout << "   2. SUPPLIERS" << endl;
    cout << "   3. PURCHASES" << endl;
    cout << "   4. EMPLOYEES" << endl;
    cout << "   5. MEMBERS" << endl;
    cout << "   6. SALES" << endl;
    cout << "   7. EXIT" << endl << endl << endl;
    cout << "Enter Your Choice : ";
    cin >> c;
    switch (c) // NOLINT(*-multiway-paths-covered)
    {
    case 1:
        system("cls");
        book_menu();
        getch();
        break;
    // TODO: Add handlers for cases 2 - 6 to call respective menus for SUPPLIERS, PURCHASES, EMPLOYEES, MEMBERS, SALES
    // TODO: Add handler for case 7 to exit the program
    // TODO: Add default case to handle invalid inputs
    default: ;
    }
}

//book menu


/**
 * @brief Displays the book menu and performs various actions based on user input.
 *
 * This function displays a menu with different options for managing books. It prompts
 * the user to enter their choice and performs the corresponding action. The available
 * options are:
 *
 *   1. ADD: Invokes the "add" method of the books class to add a new book record to the database.
 *   2. UPDATE PRICE: Invokes the "update_price" method of the books class to update the price of a book.
 *   3. SEARCH: Invokes the "search" method of the books class to search for a book based on user input.
 *   4. UPDATE STATUS: Invokes the "update" static method of the books class to update the status of books.
 *   5. DISPLAY ALL: Invokes the "display" static method of the books class to display all book records.
 *   6. RETURN TO MAIN MENU: Returns to the main menu.
 *
 * @details This function first displays the book menu options using cout. It then prompts the user to enter
 * their choice using cin. Based on the user's choice, it invokes the corresponding method of the books class.
 * If an invalid choice is entered, an error message is displayed.
 *
 * @param None
 * @return None
 */
void book_menu()
{
    int c;
    books b;

    cout << "*************************************************" << endl;
    cout << "                  BOOK MENU" << endl;
    cout << "*************************************************" << endl;
    cout << "   1. ADD" << endl;
    cout << "   2. UPDATE PRICE" << endl;
    cout << "   3. SEARCH" << endl;
    cout << "   4. UPDATE STATUS" << endl;
    cout << "   5. DISPLAY ALL" << endl;
    cout << "   6. RETURN TO MAIN MENU" << endl << endl << endl;
    cout << "Enter Your Choice : ";
    cin >> c;
    switch (c)
    {
    case 1:
        b.add();
        break;
    case 2:
        b.update_price();
        break;
    case 3:
        b.search();
        break;
    case 4:
        books::update();
        break;
    case 5:
        books::display();
        break;
    case 6:
        return;
    default:
        cout << "Wrong Input" << endl << "Invalid Input";
        break;
    }
}

//sup menu

//purchase menu

//emp menu

//mem menu

//sal menu

//pass function

/**
 * @brief Prompts the user to enter a password and checks if it is correct.
 *
 * This function prompts the user to enter a 4-digit password and checks if it matches the
 * predefined password constant.
 *
 * @note This function uses getch() and cout from the iostream library, so make sure to include it.
 * @note This function uses the PASSWORD constant, which should be defined beforehand.
 */
void pass()
{
    int num = 0;
    cout << "Enter password :";
    for (int i = 0; i < 4; i++)
    {
        num = num * 10 + (getch() - 48);
        cout << "*";
    }
    if (num == PASSWORD)
    {
        cout << endl << endl << "Correct Password" << endl << endl;
        cout << "Press any key...";
        getch();
    }
    else
    {
        cout << endl << endl << "Incorrect Password" << endl << endl;
        cout << "Press any key..";
        getch();
        exit(1);
    }
}

// MAIN FUNCTION
//TODO: Remove the [[noreturn]] attribute from the main function as it is not needed for now.
//TODO: Add the "int choice;" variable that will be used for handling user's choice.
//TODO: Add a connection status check after establishing the connection with the database.
//TODO: Use "NULL" instead of "nullptr" for C++ version compatibility.
//TODO: Add an error message to be displayed if the connection to the database is not established.
//TODO: Add a getch() function call after displaying the error message to give user a chance to read the message before the program closes.
//TODO: Prepare for halting the main loop and properly ending the program.
//TODO: Add a "return 0;" statement at the end of the main function to signal a successful program execution.
[[noreturn]] int main()
{
    pass();
    conn = mysql_init(nullptr);
    conn = mysql_real_connect(conn, HOST, USER, PASS, DATABASE, PORT, nullptr, 0);
    while (true)
    {
        system("cls");
        main_menu();
    }
}

#pragma clang diagnostic pop
