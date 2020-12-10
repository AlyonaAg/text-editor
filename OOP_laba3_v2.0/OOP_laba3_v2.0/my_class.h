#include <initializer_list>
#include <string>

class MyString
{
protected:
	char* a_string_;
	size_t a_len_string_;
	size_t a_capacity_;

	const size_t cMaxCinLength = 5000;
	const size_t cCapacityBlock = 15;

	void reallocation_of_memory(const char* string, size_t count);
public:
	MyString();										// default constructor
	MyString(const MyString& point);				// copy constructor
	MyString(const char* string);							// char array constructor
	MyString(const char* string, size_t count);				// init class with count characters of  “char string”
	MyString(size_t count, char symbol);				// init class with count of characters
	MyString(std::initializer_list <char> list);	// initializer list constructor 
	MyString(std::string& string);					// std::string constructor

	~MyString() { delete[] a_string_; a_string_ = nullptr; };

	MyString operator +(const char* string) const;
	friend MyString operator +(const char* string, const MyString& point);
	MyString operator +(const MyString& point) const;
	MyString operator +(const std::string& string) const;
	friend MyString operator +(const std::string& string, const MyString& point);

	void operator =(const char* string);
	void operator =(const std::string& string);
	void operator =(const MyString& point);
	void operator =(const char symbol);

	void operator +=(const char* string);
	void operator +=(const MyString& point);
	void operator +=(const std::string& string);

	bool operator ==(const char* string) const;
	bool operator ==(const MyString& point) const;
	bool operator ==(const std::string& string) const;
	friend bool operator ==(const char* string, const MyString& point);
	friend bool operator ==(const std::string& string, const MyString& point);

	bool operator !=(const char* string) const;
	bool operator !=(const MyString& point) const;
	bool operator !=(const std::string& string) const;
	friend bool operator !=(const char* string, const MyString& point);
	friend bool operator !=(const std::string& string, const MyString& point);

	bool operator >(const char* string) const;
	bool operator >(const std::string& string) const;
	bool operator >(const MyString& point) const;
	friend bool operator >(const char* string, const MyString& point);
	friend bool operator >(const std::string& string, const MyString& point);

	bool operator <(const char* string) const;
	bool operator <(const std::string& string) const;
	bool operator <(const MyString& point) const;
	friend bool operator <(const char* string, const MyString& point);
	friend bool operator <(const std::string& string, const MyString& point);

	bool operator >=(const char* string) const;
	bool operator >=(const std::string& string) const;
	bool operator >=(const MyString& point) const;
	friend bool operator >=(const char* string, const MyString& point);
	friend bool operator >=(const std::string& string, const MyString& point);

	bool operator <=(const char* string) const;
	bool operator <=(const std::string& string) const;
	bool operator <=(const MyString& point) const;
	friend bool operator <=(const char* string, const MyString& point);
	friend bool operator <=(const std::string& string, const MyString& point);

	const char operator[] (size_t index) const;

	friend std::ostream& operator << (std::ostream& out, const MyString& point);
	friend std::istream& operator >> (std::istream& in, MyString& point);

	const char* c_str() const;
	const char* data() const;
	size_t size() const;
	size_t length() const;
	bool empty() const;
	void clear();
	size_t capacity() const;
	void shrink_to_fit();

	void insert(size_t index, size_t count, const char symbol);
	void insert(size_t index, const char* string);
	void insert(size_t index, const std::string& string);
	void insert(size_t index, const char* string, size_t count);
	void insert(size_t index, const std::string& string, size_t count);

	void erase(size_t index, size_t count);

	void append(size_t count, const char symbol);
	void append(const MyString& point);
	void append(const char* string);
	void append(const std::string& string);
	void append(const char* string, size_t index, size_t count);
	void append(const std::string& string, size_t index, size_t count);

	void replace(size_t index, size_t count, const char* string);
	void replace(size_t index, size_t count, const std::string& string);

	MyString substr(size_t index) const;
	MyString substr(size_t index, size_t count) const;

	size_t find(const char* string) const;
	size_t find(const std::string& string) const;
	size_t find(const char* string, size_t index) const;
	size_t find(const std::string& string, size_t index) const;
};


class MyStringPython :public MyString
{
public:
	MyStringPython() :MyString() {};											// default constructor
	MyStringPython(const MyString& point) : MyString(point) {};				// copy constructor
	MyStringPython(char* string) :MyString(string) {};						// char array constructor
	MyStringPython(char* string, size_t count) :MyString(string, count) {};	// init class with count characters of  “char string”
	MyStringPython(size_t count, char symbol) :MyString(count, symbol) {};	// init class with count of characters
	MyStringPython(std::string& string) :MyString(string) {};				// std::string constructor

	~MyStringPython() {};

	using MyString::operator+;
	using MyString::operator<;
	using MyString::operator>;
	using MyString::operator<=;
	using MyString::operator>=;
	using MyString::operator==;
	using MyString::operator!=;

	MyStringPython operator +=(const char* string);
	MyStringPython operator +=(const MyStringPython& point);
	MyStringPython operator +=(const std::string& string);

	const char* __str__();
	void __setitem__(size_t index, char symbol);
	const char __getitem__(size_t index);
};
