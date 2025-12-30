#include <iostream>
#include <vector>
#include <string>
#include <sqlite3.h>
#include <iomanip>
#include <algorithm>
#include <limits>

using namespace std;

struct Book {
    int id;
    string title;
    string author;
    string isbn;
    int quantity;
    bool needsSave;
};

class LibrarySystem {
private:
    vector<Book> books;
    sqlite3* db;
    int nextId;

    static int callback(void* data, int argc, char** argv, char** azColName) {
        return 0;
    }

    bool executeSQL(const string& sql) {
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), callback, 0, &errMsg);

        if (rc != SQLITE_OK) {
            cerr << "SQL error: " << errMsg << endl;
            sqlite3_free(errMsg);
            return false;
        }
        return true;
    }

    void loadFromDatabase() {
        string sql = "SELECT * FROM books;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                Book book;
                book.id = sqlite3_column_int(stmt, 0);
                book.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                book.author = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                book.isbn = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
                book.quantity = sqlite3_column_int(stmt, 4);
                book.needsSave = false;
                books.push_back(book);

                if (book.id >= nextId) {
                    nextId = book.id + 1;
                }
            }
        }
        sqlite3_finalize(stmt);
    }

public:
    LibrarySystem() : db(nullptr), nextId(1) {
        int rc = sqlite3_open("library.db", &db);

        if (rc) {
            cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
            exit(1);
        }

        string createTable =
            "CREATE TABLE IF NOT EXISTS books ("
            "id INTEGER PRIMARY KEY, "
            "title TEXT NOT NULL, "
            "author TEXT NOT NULL, "
            "isbn TEXT NOT NULL, "
            "quantity INTEGER NOT NULL);";

        executeSQL(createTable);
        loadFromDatabase();
    }

    ~LibrarySystem() {
        if (db) {
            sqlite3_close(db);
        }
    }

    void addBook() {
        Book book;
        book.id = nextId++;
        book.needsSave = true;

        cout << "\nBook ID: " << book.id << endl;

        while (true) {
            cout << "Enter Title: ";
            getline(cin, book.title);
            if (!book.title.empty()) {
                break;
            }
            cout << "*Title cannot be empty. Title is require." << endl;
        }

        while (true) {
            cout << "Enter Author: ";
            getline(cin, book.author);
            if (!book.author.empty()) {
                break;
            }
            cout << "*Author cannot be empty. Author is needed." << endl;
        }

        while (true) {
            cout << "Enter ISBN: ";
            getline(cin, book.isbn);
            if (!book.isbn.empty()) {
                break;
            }
            cout << "*ISBN cannot be empty. ISBN is mandatory ." << endl;
        }

        while (true) {
            cout << "Enter Quantity: ";
            cin >> book.quantity;
            if (cin.fail() || book.quantity < 0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "*Invalid quantity. Please enter a valid number (0 or greater)." << endl;
            } else {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }
        }

        books.push_back(book);
        cout << "\n✓ Book added to memory! (Remember to save)" << endl;
    }

    void editBook() {
        int bookId;
        cout << "\nEnter Book ID to Edit: ";
        cin >> bookId;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        auto it = find_if(books.begin(), books.end(),
                         [bookId](const Book& b) { return b.id == bookId; });

        if (it == books.end()) {
            cout << "\n✗ Book not found!" << endl;
            return;
        }

        cout << "\nCurrent Book Information:" << endl;
        cout << "Title: " << it->title << endl;
        cout << "Author: " << it->author << endl;
        cout << "ISBN: " << it->isbn << endl;
        cout << "Quantity: " << it->quantity << endl;

        cout << "\nEnter New Title (or press Enter to keep current): ";
        string input;
        getline(cin, input);
        if (!input.empty()) it->title = input;

        cout << "Enter New Author (or press Enter to keep current): ";
        getline(cin, input);
        if (!input.empty()) it->author = input;

        cout << "Enter New ISBN (or press Enter to keep current): ";
        getline(cin, input);
        if (!input.empty()) it->isbn = input;

        while (true) {
            cout << "Enter New Quantity (or -1 to keep current): ";
            int qty;
            cin >> qty;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "*Invalid input. Please enter a valid number." << endl;
            } else {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (qty == -1) {
                    break;
                } else if (qty >= 0) {
                    it->quantity = qty;
                    break;
                } else {
                    cout << "*Quantity cannot be negative. Please try again." << endl;
                }
            }
        }

        it->needsSave = true;
        cout << "\n✓ Book updated in memory! (Remember to save)" << endl;
    }

    void deleteBook() {
        cout << "\nDelete Options:" << endl;
        cout << "1. Delete Single Book" << endl;
        cout << "2. Delete All Books" << endl;
        cout << "Enter choice: ";

        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1) {
            int bookId;
            cout << "Enter Book ID to Delete: ";
            cin >> bookId;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            auto it = find_if(books.begin(), books.end(),
                             [bookId](const Book& b) { return b.id == bookId; });

            if (it == books.end()) {
                cout << "\n✗ Book not found!" << endl;
                return;
            }

            cout << "Are you sure you want to delete '" << it->title << "'? (y/n): ";
            char confirm;
            cin >> confirm;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (confirm == 'y' || confirm == 'Y') {
                string sql = "DELETE FROM books WHERE id=" + to_string(bookId) + ";";
                if (executeSQL(sql)) {
                    books.erase(it);
                    cout << "\n✓ Book deleted successfully from database!" << endl;
                } else {
                    cout << "\n✗ Failed to delete book!" << endl;
                }
            }
        } else if (choice == 2) {
            cout << "Are you sure you want to delete ALL books? (y/n): ";
            char confirm;
            cin >> confirm;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (confirm == 'y' || confirm == 'Y') {
                if (executeSQL("DELETE FROM books;")) {
                    books.clear();
                    nextId = 1;
                    cout << "\n✓ All books deleted successfully from database!" << endl;
                } else {
                    cout << "\n✗ Failed to delete books!" << endl;
                }
            }
        }
    }

    void searchBook() {
        cout << "\nSearch By:" << endl;
        cout << "1. Book ID" << endl;
        cout << "2. Title" << endl;
        cout << "3. Author" << endl;
        cout << "Enter choice: ";

        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        vector<Book> results;

        if (choice == 1) {
            int id;
            cout << "Enter Book ID: ";
            cin >> id;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            copy_if(books.begin(), books.end(), back_inserter(results),
                   [id](const Book& b) { return b.id == id; });
        } else if (choice == 2) {
            string title;
            cout << "Enter Title: ";
            getline(cin, title);
            copy_if(books.begin(), books.end(), back_inserter(results),
                   [&title](const Book& b) {
                       return b.title.find(title) != string::npos;
                   });
        } else if (choice == 3) {
            string author;
            cout << "Enter Author: ";
            getline(cin, author);
            copy_if(books.begin(), books.end(), back_inserter(results),
                   [&author](const Book& b) {
                       return b.author.find(author) != string::npos;
                   });
        }

        if (results.empty()) {
            cout << "\n✗ No matching books found!" << endl;
        } else {
            displayBooks(results);
        }
    }

    void displayBooks(const vector<Book>& bookList) {
        cout << "\n" << string(85, '=') << endl;
        cout << left << setw(5) << "ID"
             << setw(25) << "Title"
             << setw(20) << "Author"
             << setw(20) << "ISBN"
             << setw(10) << "Quantity" << endl;
        cout << string(85, '=') << endl;

        for (const auto& book : bookList) {
            cout << left << setw(5) << book.id
                 << setw(25) << book.title.substr(0, 24)
                 << setw(20) << book.author.substr(0, 19)
                 << setw(20) << book.isbn
                 << setw(10) << book.quantity << endl;
        }
        cout << string(85, '=') << endl;
        cout << "Total Books: " << bookList.size() << endl;
    }

    void showAllBooks() {
        if (books.empty()) {
            cout << "\n✗ Library is empty!" << endl;
        } else {
            displayBooks(books);
        }
    }

    void saveData() {
        int savedCount = 0;
        int updatedCount = 0;

        for (auto& book : books) {
            if (book.needsSave) {
                string checkSql = "SELECT id FROM books WHERE id=" + to_string(book.id) + ";";
                sqlite3_stmt* stmt;
                bool exists = false;

                if (sqlite3_prepare_v2(db, checkSql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                    if (sqlite3_step(stmt) == SQLITE_ROW) {
                        exists = true;
                    }
                }
                sqlite3_finalize(stmt);

                string sql;
                if (exists) {
                    sql = "UPDATE books SET title='" + book.title +
                          "', author='" + book.author +
                          "', isbn='" + book.isbn +
                          "', quantity=" + to_string(book.quantity) +
                          " WHERE id=" + to_string(book.id) + ";";
                    if (executeSQL(sql)) {
                        updatedCount++;
                        book.needsSave = false;
                    }
                } else {
                    sql = "INSERT INTO books (id, title, author, isbn, quantity) VALUES (" +
                          to_string(book.id) + ", '" + book.title + "', '" +
                          book.author + "', '" + book.isbn + "', " +
                          to_string(book.quantity) + ");";
                    if (executeSQL(sql)) {
                        savedCount++;
                        book.needsSave = false;
                    }
                }
            }
        }

        cout << "\n✓ Save completed!" << endl;
        cout << "New books saved: " << savedCount << endl;
        cout << "Books updated: " << updatedCount << endl;
        cout << "Total books in database: " << books.size() << endl;
    }

    void displayMenu() {
        cout << "\n╔══════════════════════════════════╗" << endl;
        cout << "║   LIBRARY MANAGEMENT SYSTEM      ║" << endl;
        cout << "╚══════════════════════════════════╝" << endl;
        cout << "1. Add Book" << endl;
        cout << "2. Edit Book" << endl;
        cout << "3. Delete Book" << endl;
        cout << "4. Search Book" << endl;
        cout << "5. Show All Books" << endl;
        cout << "6. Save Data" << endl;
        cout << "7. Exit" << endl;
        cout << "\nEnter your choice: ";
    }

    void run() {
        int choice;

        while (true) {
            displayMenu();
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
                case 1:
                    addBook();
                    break;
                case 2:
                    editBook();
                    break;
                case 3:
                    deleteBook();
                    break;
                case 4:
                    searchBook();
                    break;
                case 5:
                    showAllBooks();
                    break;
                case 6:
                    saveData();
                    break;
                case 7:
                    cout << "\nExiting..." << endl;
                    cout << "Thank you for using Library Management System!" << endl;
                    return;
                default:
                    cout << "\n✗ Invalid choice! Please try again." << endl;
            }
        }
    }
};

int main() {
    LibrarySystem library;
    library.run();
    return 0;
}