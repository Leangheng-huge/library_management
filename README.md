# 📚 Library Management System

A colorful command-line library management system built with C++ and SQLite3.

![C++](https://img.shields.io/badge/C++-11-blue.svg)
![SQLite](https://img.shields.io/badge/SQLite-3-green.svg)
![License](https://img.shields.io/badge/license-MIT-blue.svg)

## ✨ Features

- 📖 Add, edit, and delete books
- 🔍 Search by ID, title, or author
- 📊 View all books in a formatted table
- 💾 SQLite database persistence
- 🎨 Colorful terminal UI

## 🚀 Quick Start

### Prerequisites
```bash
# Ubuntu/Debian
sudo apt-get install build-essential libsqlite3-dev

# macOS
xcode-select --install
brew install sqlite3
```

### Build & Run
```bash
g++ -std=c++11 library.cpp -o library_system -lsqlite3
./library_system
```

## 📖 Usage

1. **Add Book** - Enter title, author, ISBN, and quantity
2. **Edit Book** - Update existing book details
3. **Delete Book** - Remove books from library
4. **Search** - Find books by ID, title, or author
5. **View All** - Display all books in table format
6. **Save** - Persist changes to database

> **Note:** Remember to save (option 6) after making changes!

## 🗂️ Database

SQLite database (`library.db`) stores:
- Book ID (auto-increment)
- Title, Author, ISBN
- Quantity

## 👤 Author

**Leangheng Korn**
- GitHub: [@Leangheng-huge](https://github.com/Leangheng-huge)

## 📜 License

MIT License - feel free to use and modify!

---

⭐ Star this repo if you find it helpful!
