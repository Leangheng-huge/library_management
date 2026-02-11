# 📚 Library Management System

A simple and colorful command-line Library Management System developed in C++ with SQLite3 for database storage.

## ✨ Key Features

- 📖 Add, update, and delete books
- 🔎 Search books by ID, title, or author
- 📋 Display all books in a formatted table
- 💾 Persistent storage using SQLite database
- 🎨 Clean and colorful terminal interface

## 🚀 Getting Started

### 🔧 Prerequisites

Make sure you have the required tools installed:

**Ubuntu / Debian**
```bash
sudo apt-get install build-essential libsqlite3-dev
```

**macOS**
```bash
xcode-select --install
brew install sqlite3
```

**Windows (MinGW)**
```bash
# Install MinGW-w64 and download SQLite3 from sqlite.org
# Add SQLite3 to your include and lib paths
```

### 📥 Clone the Repository

```bash
git clone https://github.com/Leangheng-huge/library-management-system.git
cd library-management-system
```

### 🛠 Build and Run

**Compile the program:**
```bash
g++ -std=c++11 library.cpp -o library_system -lsqlite3
```

**Run the application:**
```bash
./library_system
```

## 📖 How to Use

1. **Add Book** – Enter title, author, ISBN, and quantity
2. **Edit Book** – Modify existing book information
3. **Delete Book** – Remove a book from the system
4. **Search Book** – Find books by ID, title, or author
5. **View All Books** – Display the full library collection
6. **Save Changes** – Store updates in the database

⚠️ **Important:** Always choose the Save option after making changes to ensure your data is stored permanently.

## 🗄 Database Structure

The system uses an SQLite database file (`library.db`) containing:

| Field    | Type         | Description              |
|----------|--------------|--------------------------|
| Book ID  | INTEGER      | Auto-increment primary key |
| Title    | TEXT         | Book title               |
| Author   | TEXT         | Book author              |
| ISBN     | TEXT         | ISBN number              |
| Quantity | INTEGER      | Available copies         |

## 📸 Screenshots

*(Add screenshots of your application here)*

## 🛣 Roadmap

- [ ] User authentication system
- [ ] Book borrowing and return functionality
- [ ] Due date tracking and notifications
- [ ] Export data to CSV/PDF
- [ ] GUI version using Qt or similar

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the project
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📝 License

This project is open source and available under the [MIT License](LICENSE).

##  Created By:

**Leangheng Korn**  
GitHub: [@Leangheng-huge](https://github.com/Leangheng-huge)

## 🙏 Acknowledgments

- SQLite for the lightweight database engine
- The C++ community for excellent documentation and support

---

⭐ If you found this project helpful, please consider giving it a star!
