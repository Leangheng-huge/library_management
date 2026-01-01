#include <iostream>
#include <vector>
#include <string>
#include <sqlite3.h>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <cstdlib>
#include <ctime>

using namespace std;

// ANSI color codes
const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN = "\033[36m";
const string WHITE = "\033[37m";
const string BOLD = "\033[1m";
const string BG_DARK = "\033[40m";

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
    vector<string> colors = {RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN};

    static int callback(void* data, int argc, char** argv, char** azColName) {
        return 0;
    }

    string getRandomColor() {
        return colors[rand() % colors.size()];
    }

    void printHeader() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        string color = getRandomColor();

        cout << color << BOLD;
        cout << "\n";
        cout << " ╦  ╦╔╗ ╦═╗╔═╗╦═╗╦ ╦  ╔╦╗╔═╗╔╗╔╔═╗╔═╗╔═╗╔╦╗╔═╗╔╗╔╔╦╗\n";
        cout << " ║  ║╠╩╗╠╦╝╠═╣╠╦╝╚╦╝  ║║║╠═╣║║║╠═╣║ ╦║╣ ║║║║╣ ║║║ ║ \n";
        cout << " ╩═╝╩╚═╝╩╚═╩ ╩╩╚═ ╩   ╩ ╩╩ ╩╝╚╝╩ ╩╚═╝╚═╝╩ ╩╚═╝╝╚╝ ╩ \n";
        cout << RESET;

        cout << CYAN << "\n ════════════════════════════════════════════════════\n" << RESET;
        cout << YELLOW << "    📚 Welcome to the Digital Library System 📚\n" << RESET;
        cout << CYAN << " ════════════════════════════════════════════════════\n" << RESET;
    }

    void printBox(const string& text, const string& color = CYAN) {
        int width = 50;
        cout << color << " ╔" << string(width, '═') << "╗\n";
        cout << " ║" << BOLD << setw((width + text.length()) / 2) << text
             << setw(width - (width + text.length()) / 2) << "" << RESET << color << "║\n";
        cout << " ╚" << string(width, '═') << "╝" << RESET << "\n";
    }

    bool executeSQL(const string& sql) {
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), callback, 0, &errMsg);

        if (rc != SQLITE_OK) {
            cout << RED << " ✗ SQL error: " << errMsg << RESET << endl;
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
        srand(time(0));

        int rc = sqlite3_open("library.db", &db);

        if (rc) {
            cerr << RED << " ✗ Can't open database: " << sqlite3_errmsg(db) << RESET << endl;
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
        printHeader();
        printBox("ADD NEW BOOK", GREEN);

        Book book;
        book.id = nextId++;
        book.needsSave = true;

        cout << GREEN << "\n 📖 Book ID: " << BOLD << book.id << RESET << endl;

        while (true) {
            cout << CYAN << " ▸ " << RESET << "Enter Title: ";
            getline(cin, book.title);
            if (!book.title.empty()) {
                break;
            }
            cout << RED << " ✗ Title cannot be empty!" << RESET << endl;
        }

        while (true) {
            cout << CYAN << " ▸ " << RESET << "Enter Author: ";
            getline(cin, book.author);
            if (!book.author.empty()) {
                break;
            }
            cout << RED << " ✗ Author cannot be empty!" << RESET << endl;
        }

        while (true) {
            cout << CYAN << " ▸ " << RESET << "Enter ISBN: ";
            getline(cin, book.isbn);
            if (!book.isbn.empty()) {
                break;
            }
            cout << RED << " ✗ ISBN cannot be empty!" << RESET << endl;
        }

        while (true) {
            cout << CYAN << " ▸ " << RESET << "Enter Quantity: ";
            cin >> book.quantity;
            if (cin.fail() || book.quantity < 0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << RED << " ✗ Invalid quantity!" << RESET << endl;
            } else {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }
        }

        books.push_back(book);
        cout << GREEN << "\n ✓ Book added successfully! (Remember to save)" << RESET << endl;
        cout << "\n Press Enter to continue...";
        cin.get();
    }

    void editBook() {
        printHeader();
        printBox("EDIT BOOK", YELLOW);

        int bookId;
        cout << CYAN << "\n ▸ " << RESET << "Enter Book ID to Edit: ";
        cin >> bookId;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        auto it = find_if(books.begin(), books.end(),
                         [bookId](const Book& b) { return b.id == bookId; });

        if (it == books.end()) {
            cout << RED << "\n ✗ Book not found!" << RESET << endl;
            cout << "\n Press Enter to continue...";
            cin.get();
            return;
        }

        cout << YELLOW << "\n Current Information:" << RESET << endl;
        cout << " Title: " << BOLD << it->title << RESET << endl;
        cout << " Author: " << BOLD << it->author << RESET << endl;
        cout << " ISBN: " << BOLD << it->isbn << RESET << endl;
        cout << " Quantity: " << BOLD << it->quantity << RESET << endl;

        cout << CYAN << "\n ▸ " << RESET << "New Title (Enter to keep): ";
        string input;
        getline(cin, input);
        if (!input.empty()) it->title = input;

        cout << CYAN << " ▸ " << RESET << "New Author (Enter to keep): ";
        getline(cin, input);
        if (!input.empty()) it->author = input;

        cout << CYAN << " ▸ " << RESET << "New ISBN (Enter to keep): ";
        getline(cin, input);
        if (!input.empty()) it->isbn = input;

        while (true) {
            cout << CYAN << " ▸ " << RESET << "New Quantity (-1 to keep): ";
            int qty;
            cin >> qty;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << RED << " ✗ Invalid input!" << RESET << endl;
            } else {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (qty == -1) {
                    break;
                } else if (qty >= 0) {
                    it->quantity = qty;
                    break;
                } else {
                    cout << RED << " ✗ Quantity cannot be negative!" << RESET << endl;
                }
            }
        }

        it->needsSave = true;
        cout << GREEN << "\n ✓ Book updated! (Remember to save)" << RESET << endl;
        cout << "\n Press Enter to continue...";
        cin.get();
    }

    void deleteBook() {
        printHeader();
        printBox("DELETE BOOK", RED);

        cout << "\n" << YELLOW << " [1]" << RESET << " Delete Single Book\n";
        cout << YELLOW << " [2]" << RESET << " Delete All Books\n";
        cout << CYAN << "\n ▸ " << RESET << "Enter choice: ";

        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1) {
            int bookId;
            cout << CYAN << " ▸ " << RESET << "Enter Book ID: ";
            cin >> bookId;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            auto it = find_if(books.begin(), books.end(),
                             [bookId](const Book& b) { return b.id == bookId; });

            if (it == books.end()) {
                cout << RED << "\n ✗ Book not found!" << RESET << endl;
                cout << "\n Press Enter to continue...";
                cin.get();
                return;
            }

            cout << RED << "\n ⚠ Delete '" << it->title << "'? (y/n): " << RESET;
            char confirm;
            cin >> confirm;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (confirm == 'y' || confirm == 'Y') {
                string sql = "DELETE FROM books WHERE id=" + to_string(bookId) + ";";
                if (executeSQL(sql)) {
                    books.erase(it);
                    cout << GREEN << "\n ✓ Book deleted!" << RESET << endl;
                }
            }
        } else if (choice == 2) {
            cout << RED << "\n ⚠ Delete ALL books? (y/n): " << RESET;
            char confirm;
            cin >> confirm;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (confirm == 'y' || confirm == 'Y') {
                if (executeSQL("DELETE FROM books;")) {
                    books.clear();
                    nextId = 1;
                    cout << GREEN << "\n ✓ All books deleted!" << RESET << endl;
                }
            }
        }
        cout << "\n Press Enter to continue...";
        cin.get();
    }

    void searchBook() {
        printHeader();
        printBox("SEARCH BOOKS", MAGENTA);

        cout << "\n" << YELLOW << " [1]" << RESET << " By Book ID\n";
        cout << YELLOW << " [2]" << RESET << " By Title\n";
        cout << YELLOW << " [3]" << RESET << " By Author\n";
        cout << CYAN << "\n ▸ " << RESET << "Enter choice: ";

        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        vector<Book> results;

        if (choice == 1) {
            int id;
            cout << CYAN << " ▸ " << RESET << "Enter Book ID: ";
            cin >> id;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            copy_if(books.begin(), books.end(), back_inserter(results),
                   [id](const Book& b) { return b.id == id; });
        } else if (choice == 2) {
            string title;
            cout << CYAN << " ▸ " << RESET << "Enter Title: ";
            getline(cin, title);
            copy_if(books.begin(), books.end(), back_inserter(results),
                   [&title](const Book& b) {
                       return b.title.find(title) != string::npos;
                   });
        } else if (choice == 3) {
            string author;
            cout << CYAN << " ▸ " << RESET << "Enter Author: ";
            getline(cin, author);
            copy_if(books.begin(), books.end(), back_inserter(results),
                   [&author](const Book& b) {
                       return b.author.find(author) != string::npos;
                   });
        }

        if (results.empty()) {
            cout << RED << "\n ✗ No matching books found!" << RESET << endl;
        } else {
            displayBooks(results);
        }
        cout << "\n Press Enter to continue...";
        cin.get();
    }

    void displayBooks(const vector<Book>& bookList) {
        cout << "\n" << CYAN << " ╔" << string(95, '═') << "╗" << RESET << endl;
        cout << CYAN << " ║ " << RESET << BOLD << left << setw(5) << "ID"
             << setw(28) << "Title"
             << setw(22) << "Author"
             << setw(20) << "ISBN"
             << setw(10) << "Quantity" << RESET << CYAN << "   ║" << RESET << endl;
        cout << CYAN << " ╠" << string(95, '═') << "╣" << RESET << endl;

        for (const auto& book : bookList) {
            cout << CYAN << " ║ " << RESET << left
                 << GREEN << setw(5) << book.id << RESET
                 << setw(28) << book.title.substr(0, 27)
                 << setw(22) << book.author.substr(0, 21)
                 << YELLOW << setw(20) << book.isbn << RESET
                 << MAGENTA << setw(10) << book.quantity << RESET
                 << CYAN << "   ║" << RESET << endl;
        }
        cout << CYAN << " ╚" << string(95, '═') << "╝" << RESET << endl;
        cout << BOLD << " 📚 Total Books: " << bookList.size() << RESET << endl;
    }

    void showAllBooks() {
        printHeader();
        printBox("ALL BOOKS IN LIBRARY", BLUE);

        if (books.empty()) {
            cout << RED << "\n ✗ Library is empty!" << RESET << endl;
            cout << "\n Press Enter to continue...";
            cin.get();
        } else {
            displayBooks(books);
            cout << "\n Press Enter to continue...";
            cin.get();
        }
    }

    void saveData() {
        printHeader();
        printBox("SAVING DATA", GREEN);

        int savedCount = 0;
        int updatedCount = 0;

        cout << YELLOW << "\n ⏳ Saving changes..." << RESET << endl;

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

        cout << GREEN << "\n ✓ Save completed!" << RESET << endl;
        cout << CYAN << " ▸ " << RESET << "New books saved: " << BOLD << savedCount << RESET << endl;
        cout << CYAN << " ▸ " << RESET << "Books updated: " << BOLD << updatedCount << RESET << endl;
        cout << CYAN << " ▸ " << RESET << "Total in database: " << BOLD << books.size() << RESET << endl;
        cout << "\n Press Enter to continue...";
        cin.get();
    }

    void displayMenu() {
        printHeader();

        cout << "\n";
        cout << CYAN << " ┌─────────────────────────────────────────────────┐\n" << RESET;
        cout << CYAN << " │" << RESET << BOLD << "              MAIN MENU OPTIONS                  " << RESET << CYAN << "│\n" << RESET;
        cout << CYAN << " └─────────────────────────────────────────────────┘\n" << RESET;

        cout << "\n";
        cout << GREEN << "  [1] " << RESET << "📖 Add Book\n";
        cout << YELLOW << "  [2] " << RESET << "✏️  Edit Book\n";
        cout << RED << "  [3] " << RESET << "🗑️  Delete Book\n";
        cout << MAGENTA << "  [4] " << RESET << "🔍 Search Book\n";
        cout << BLUE << "  [5] " << RESET << "📚 Show All Books\n";
        cout << GREEN << "  [6] " << RESET << "💾 Save Data\n";
        cout << RED << "  [7] " << RESET << "🚪 Exit\n";

        cout << "\n" << CYAN << " ▸ " << RESET << "Enter your choice: ";
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
                    printHeader();
                    printBox("THANK YOU!", GREEN);
                    cout << GREEN << "\n ✓ Exiting Library Management System...\n" << RESET;
                    cout << CYAN << "   See you next time! 📚✨\n\n" << RESET;
                    return;
                default:
                    printHeader();
                    cout << RED << "\n ✗ Invalid choice! Please try again." << RESET << endl;
                    cout << "\n Press Enter to continue...";
                    cin.get();
            }
        }
    }
};

int main() {
    LibrarySystem library;
    library.run();
    return 0;
}